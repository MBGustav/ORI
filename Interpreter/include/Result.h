#pragma once

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "baseline_common.h"

/*
 * Result.h
 *
 * What a statement produces: either a message (DDL / INSERT) or a result set
 * (SELECT). The REPL prints toString(); tests can inspect columns/rows directly.
 */

inline std::string valueToString(const Value& value) {
    switch (value.index()) {
        case 0: return std::to_string(std::get<int64_t>(value));
        case 1: return std::to_string(std::get<double>(value));
        case 2: return std::get<bool>(value) ? "true" : "false";
        case 3: return std::get<std::string>(value);
        default: return {};
    }
}

struct Result {
    std::vector<std::string> columns;
    std::vector<std::vector<Value>> rows;
    std::string message;

    bool hasRows() const { return !columns.empty(); }

    std::string toString() const {
        if (columns.empty()) {
            return message;
        }

        std::vector<std::size_t> widths(columns.size());
        for (std::size_t column = 0; column < columns.size(); ++column) {
            widths[column] = columns[column].size();
        }
        for (const auto& row : rows) {
            for (std::size_t column = 0; column < row.size(); ++column) {
                widths[column] =
                    std::max(widths[column], valueToString(row[column]).size());
            }
        }

        auto separator = [&]() {
            std::string line = "+";
            for (const std::size_t width : widths) {
                line += std::string(width + 2, '-');
                line += "+";
            }
            return line;
        };

        auto formatRow = [&](const std::vector<std::string>& cells) {
            std::string line = "|";
            for (std::size_t column = 0; column < cells.size(); ++column) {
                line += " " + cells[column];
                line += std::string(widths[column] - cells[column].size(), ' ');
                line += " |";
            }
            return line;
        };

        std::ostringstream out;
        out << separator() << "\n";
        out << formatRow(columns) << "\n";
        out << separator() << "\n";
        for (const auto& row : rows) {
            std::vector<std::string> cells;
            cells.reserve(row.size());
            for (const Value& value : row) {
                cells.push_back(valueToString(value));
            }
            out << formatRow(cells) << "\n";
        }
        out << separator() << "\n";
        out << rows.size() << " row(s) selected.\n";
        return out.str();
    }
};
