#pragma once

#include <string>
#include <variant>
#include <vector>

#include "baseline_common.h"

/*
 * AST.h
 *
 * The abstract syntax tree produced by the Parser. At this scope each statement
 * is a plain struct and a Statement is a std::variant over them; the Executor
 * dispatches with std::visit / holds over the variant.
 *
 * Supported statements (milestone 1):
 *   CREATE TABLE t (col TYPE [INDEXED], ...)
 *   DROP TABLE t
 *   INSERT INTO t VALUES (v1, v2, ...)
 *   SELECT * | c1, c2 FROM t [WHERE col OP literal]
 */

// Comparison operators allowed in a WHERE clause.
enum class ComparisonOp {
    Eq,
    Neq,
    Lt,
    Le,
    Gt,
    Ge
};

// A column in a CREATE TABLE statement.
struct ColumnDef {
    std::string name;
    DataType type;
    bool indexed;

    ColumnDef(std::string columnName, DataType columnType, bool isIndexed)
        : name(std::move(columnName)), type(columnType), indexed(isIndexed) {}
};

struct CreateTableStmt {
    std::string table;
    std::vector<ColumnDef> columns;
};

struct DropTableStmt {
    std::string table;
};

struct InsertStmt {
    std::string table;
    std::vector<Value> values;
};

struct WhereClause {
    std::string column;
    ComparisonOp op;
    Value value;
};

struct SelectStmt {
    std::string table;
    bool selectAll;                    // true for "SELECT *"
    std::vector<std::string> columns;  // populated when !selectAll
    bool hasWhere;
    WhereClause where;

    SelectStmt() : selectAll(true), hasWhere(false), where{"", ComparisonOp::Eq, Value{}} {}
};

using Statement = std::variant<
    CreateTableStmt,
    DropTableStmt,
    InsertStmt,
    SelectStmt
>;
