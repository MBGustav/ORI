#include "SchemaFile.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "SqlError.h"

namespace {

std::string toUpperAscii(std::string value) {
    for (char& character : value) {
        character = static_cast<char>(
            std::toupper(static_cast<unsigned char>(character)));
    }
    return value;
}

std::vector<std::string> splitWhitespace(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> parts;
    std::string part;
    while (stream >> part) {
        parts.push_back(part);
    }
    return parts;
}

}  // namespace

namespace SchemaFile {

std::string dataTypeName(DataType type) {
    switch (type) {
        case DataType::Integer: return "INTEGER";
        case DataType::Varchar: return "VARCHAR";
    }
    return "VARCHAR";
}

DataType parseDataType(const std::string& text) {
    const std::string upper = toUpperAscii(text);
    if (upper == "INT" || upper == "INTEGER") {
        return DataType::Integer;
    }
    if (upper == "VARCHAR" || upper == "STRING" || upper == "TEXT") {
        return DataType::Varchar;
    }
    throw SqlError("unknown data type in schema: " + text);
}

void write(const std::string& path, const TableDefinition& definition) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        throw std::runtime_error("SchemaFile: cannot write " + path);
    }

    out << "table " << definition.name << "\n";
    for (const Column& column : definition.columns) {
        out << "column " << column.name << " "
            << dataTypeName(column.type);
        if (column.indexed) {
            out << " INDEXED";
        }
        out << "\n";
    }
}

TableDefinition read(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("SchemaFile: cannot read " + path);
    }

    TableDefinition definition;
    bool sawTable = false;
    std::string line;

    while (std::getline(in, line)) {
        const std::vector<std::string> parts = splitWhitespace(line);
        if (parts.empty()) {
            continue;
        }

        const std::string keyword = toUpperAscii(parts[0]);
        if (keyword == "TABLE") {
            if (parts.size() != 2) {
                throw SqlError("malformed schema header in " + path);
            }
            definition.name = parts[1];
            sawTable = true;
            continue;
        }

        if (keyword == "COLUMN") {
            if (!sawTable) {
                throw SqlError("schema column before table header in " + path);
            }
            if (parts.size() < 3) {
                throw SqlError("malformed column line in " + path);
            }
            const std::string& name = parts[1];
            const DataType type = parseDataType(parts[2]);
            const bool indexed = parts.size() >= 4 &&
                                 toUpperAscii(parts[3]) == "INDEXED";
            definition.columns.push_back(Column(name, type, indexed));
            continue;
        }

        throw SqlError("unknown schema line in " + path + ": " + line);
    }

    if (!sawTable) {
        throw SqlError("schema file has no table header: " + path);
    }
    if (definition.columns.empty()) {
        throw SqlError("schema file has no columns: " + path);
    }

    return definition;
}

}  // namespace SchemaFile
