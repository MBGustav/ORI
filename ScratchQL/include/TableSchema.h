#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "baseline_common.h"
#include "BinPlusTree.h"

/*
 * TableSchema.h
 *
 * The "TableSchema" layer of the architecture:
 *
 *   TableSchema
 *       |
 *     Indexes
 *       |
 *       +-- B+Tree (column 0)
 *       +-- B+Tree (column 1)
 *       ...
 */

// A single column definition. `indexed` marks the columns that get a B+Tree.
struct Column {
    std::string name;
    DataType type;
    bool indexed;

    Column(std::string columnName, DataType columnType, bool isIndexed = false)
        : name(std::move(columnName)), type(columnType), indexed(isIndexed) {}
};

// The "Indexes" layer: one B+Tree per column. Only columns flagged `indexed`
// are ever populated; the others keep an empty tree.
class Indexes {
public:
    Indexes() = default;
    Indexes(const Indexes&) = default;
    Indexes& operator=(const Indexes&) = default;
    Indexes(Indexes&&) noexcept = default;
    Indexes& operator=(Indexes&&) noexcept = default;

    void resize(std::size_t columnCount) {
        trees_.resize(columnCount);
    }

    BinPlusTree& operator[](std::size_t columnId) {
        return trees_.at(columnId);
    }

    const BinPlusTree& operator[](std::size_t columnId) const {
        return trees_.at(columnId);
    }

    std::size_t size() const {
        return trees_.size();
    }

private:
    std::vector<BinPlusTree> trees_;
};

// The "TableSchema" layer: column definitions plus their indexes.
struct TableSchema {
    std::vector<Column> columns;
    Indexes indexes;

    TableSchema() = default;

    explicit TableSchema(const std::vector<Column>& cols)
        : columns(cols) {
        indexes.resize(columns.size());
    }

    std::size_t columnCount() const {
        return columns.size();
    }
};

// Converts a stored value into the string key used by the B+Trees.
inline std::string valueToKey(const Value& value) {
    switch (value.index()) {
        case 0: return std::to_string(std::get<int64_t>(value));
        case 1: return std::to_string(std::get<double>(value));
        case 2: return std::get<bool>(value) ? "1" : "0";
        case 3: return std::get<std::string>(value);
        default: return {};
    }
}
