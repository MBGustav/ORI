#include "Executor.h"

#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "AST.h"
#include "Catalog.h"
#include "Result.h"
#include "SqlError.h"
#include "TableSchema.h"
#include "TableWrapper.h"
#include "baseline_common.h"

namespace {

std::size_t columnIndex(const TableSchema& schema, const std::string& name) {
    for (std::size_t index = 0; index < schema.columns.size(); ++index) {
        if (schema.columns[index].name == name) {
            return index;
        }
    }
    throw SqlError("unknown column '" + name + "'");
}

bool isNumeric(const Value& value) {
    return value.index() == 0 || value.index() == 1;
}

double asDouble(const Value& value) {
    if (value.index() == 0) {
        return static_cast<double>(std::get<int64_t>(value));
    }
    return std::get<double>(value);
}

int compareValues(const Value& left, const Value& right) {
    if (isNumeric(left) && isNumeric(right)) {
        const double a = asDouble(left);
        const double b = asDouble(right);
        if (a < b) return -1;
        if (a > b) return 1;
        return 0;
    }
    if (left.index() == 3 && right.index() == 3) {
        const std::string& a = std::get<std::string>(left);
        const std::string& b = std::get<std::string>(right);
        if (a < b) return -1;
        if (a > b) return 1;
        return 0;
    }
    if (left.index() == 2 && right.index() == 2) {
        const bool a = std::get<bool>(left);
        const bool b = std::get<bool>(right);
        return (a == b) ? 0 : (a ? 1 : -1);
    }
    throw SqlError("cannot compare values of different types");
}

bool matches(int comparison, ComparisonOp op) {
    switch (op) {
        case ComparisonOp::Eq: return comparison == 0;
        case ComparisonOp::Neq: return comparison != 0;
        case ComparisonOp::Lt: return comparison < 0;
        case ComparisonOp::Le: return comparison <= 0;
        case ComparisonOp::Gt: return comparison > 0;
        case ComparisonOp::Ge: return comparison >= 0;
    }
    return false;
}

// Converts a literal to the representation stored for @p column, rejecting
// values that do not fit the column type.
Value coerceForColumn(const Value& literal, const Column& column) {
    if (column.type == DataType::Integer) {
        if (std::holds_alternative<int64_t>(literal)) {
            return literal;
        }
        if (std::holds_alternative<double>(literal)) {
            const double value = std::get<double>(literal);
            if (value == static_cast<double>(static_cast<int64_t>(value))) {
                return Value{static_cast<int64_t>(value)};
            }
        }
        throw SqlError("column '" + column.name + "' expects an integer value");
    }

    // Varchar
    if (std::holds_alternative<std::string>(literal)) {
        return literal;
    }
    throw SqlError("column '" + column.name + "' expects a string value");
}

}  // namespace

Result Executor::execute(const Statement& statement) {
    if (const auto* create = std::get_if<CreateTableStmt>(&statement)) {
        return executeCreate(*create);
    }
    if (const auto* drop = std::get_if<DropTableStmt>(&statement)) {
        return executeDrop(*drop);
    }
    if (const auto* insert = std::get_if<InsertStmt>(&statement)) {
        return executeInsert(*insert);
    }
    if (const auto* select = std::get_if<SelectStmt>(&statement)) {
        return executeSelect(*select);
    }
    throw SqlError("unsupported statement");
}

Result Executor::executeCreate(const CreateTableStmt& statement) {
    std::set<std::string> seen;
    std::vector<Column> columns;
    columns.reserve(statement.columns.size());

    for (const ColumnDef& def : statement.columns) {
        if (!seen.insert(def.name).second) {
            throw SqlError("duplicate column '" + def.name + "'");
        }
        columns.push_back(Column(def.name, def.type, def.indexed));
    }

    catalog_.create(statement.table, columns);

    Result result;
    result.message = "Table '" + statement.table + "' created.";
    return result;
}

Result Executor::executeDrop(const DropTableStmt& statement) {
    catalog_.drop(statement.table);

    Result result;
    result.message = "Table '" + statement.table + "' dropped.";
    return result;
}

Result Executor::executeInsert(const InsertStmt& statement) {
    TableWrapper& table = catalog_.open(statement.table);
    const TableSchema& schema = table.schema();

    if (statement.values.size() != schema.columns.size()) {
        throw SqlError(
            "INSERT for '" + statement.table + "' has " +
            std::to_string(statement.values.size()) + " value(s) but the table has " +
            std::to_string(schema.columns.size()) + " column(s)");
    }

    std::vector<Value> row;
    row.reserve(statement.values.size());
    for (std::size_t index = 0; index < statement.values.size(); ++index) {
        row.push_back(coerceForColumn(statement.values[index], schema.columns[index]));
    }

    table.insertRecord(row);

    Result result;
    result.message = "1 row inserted into '" + statement.table + "'.";
    return result;
}

Result Executor::executeSelect(const SelectStmt& statement) {
    TableWrapper& table = catalog_.open(statement.table);
    const TableSchema& schema = table.schema();

    // Resolve the projected columns.
    std::vector<std::size_t> projected;
    std::vector<std::string> columnNames;
    if (statement.selectAll) {
        for (std::size_t index = 0; index < schema.columns.size(); ++index) {
            projected.push_back(index);
            columnNames.push_back(schema.columns[index].name);
        }
    } else {
        for (const std::string& name : statement.columns) {
            projected.push_back(columnIndex(schema, name));
            columnNames.push_back(name);
        }
    }

    // Resolve and coerce the WHERE clause, if any.
    bool hasWhere = statement.hasWhere;
    std::size_t whereIndex = 0;
    Value whereValue{};
    if (hasWhere) {
        whereIndex = columnIndex(schema, statement.where.column);
        whereValue = coerceForColumn(statement.where.value, schema.columns[whereIndex]);
    }

    // Choose the candidate rows: index lookup for equality on an indexed
    // column, otherwise a full scan.
    const bool useIndex = hasWhere &&
                          statement.where.op == ComparisonOp::Eq &&
                          schema.columns[whereIndex].indexed;

    std::vector<RecordID> candidateIds;
    if (useIndex) {
        candidateIds = table.find(statement.where.column, whereValue);
    } else {
        const std::size_t total = table.recordCount();
        candidateIds.reserve(total);
        for (RecordID id = 0; id < total; ++id) {
            candidateIds.push_back(id);
        }
    }

    Result result;
    result.columns = columnNames;

    for (const RecordID id : candidateIds) {
        const Record& record = table.recordStore().at(id);

        if (hasWhere) {
            const int comparison = compareValues(record.values[whereIndex], whereValue);
            if (!matches(comparison, statement.where.op)) {
                continue;
            }
        }

        std::vector<Value> row;
        row.reserve(projected.size());
        for (const std::size_t index : projected) {
            row.push_back(record.values[index]);
        }
        result.rows.push_back(std::move(row));
    }

    return result;
}
