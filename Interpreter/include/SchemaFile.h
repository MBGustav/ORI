#pragma once

#include <string>
#include <vector>

#include "TableSchema.h"

/*
 * SchemaFile.h
 *
 * Persists a table's schema next to its data file as plain text, so a table can
 * be reopened after the interpreter exits. Format:
 *
 *   table people
 *   column id INTEGER INDEXED
 *   column nome VARCHAR
 *
 * Only metadata is written here; the rows live in the ".db" file managed by
 * RecordStore.
 */

struct TableDefinition {
    std::string name;
    std::vector<Column> columns;
};

namespace SchemaFile {

// Human-readable name of a DataType (INTEGER / VARCHAR).
std::string dataTypeName(DataType type);

// Parses a type keyword (case-insensitive). Throws SqlError if unknown.
DataType parseDataType(const std::string& text);

// Writes the definition to @p path, overwriting any existing file.
void write(const std::string& path, const TableDefinition& definition);

// Reads a definition from @p path. Throws SqlError if the file is malformed.
TableDefinition read(const std::string& path);

}  // namespace SchemaFile
