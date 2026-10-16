#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "SchemaFile.h"
#include "TableSchema.h"
#include "TableWrapper.h"

/*
 * Catalog.h
 *
 * Owns a directory of tables. Each table is a pair of files:
 *   <root>/<name>.schema   (see SchemaFile)
 *   <root>/<name>.db       (rows, managed by RecordStore)
 *
 * The catalog loads every schema on construction and keeps an open
 * TableWrapper per table on demand. TableWrapper is neither copyable nor
 * movable (it embeds an open RecordStore), so it is held behind a unique_ptr
 * and there is at most one open handle per table.
 */
class Catalog {
public:
    explicit Catalog(std::string rootDir);

    bool exists(const std::string& name) const;

    // Table names, sorted alphabetically.
    std::vector<std::string> tableNames() const;

    // Schema of @p name. Throws SqlError if the table does not exist.
    const TableDefinition& definition(const std::string& name) const;

    // Opens (or returns the cached) wrapper for @p name. Throws if missing.
    TableWrapper& open(const std::string& name);

    // Creates the schema file for a new table. Throws if it already exists.
    void create(const std::string& name, const std::vector<Column>& columns);

    // Closes and deletes both files of @p name. Throws if missing.
    void drop(const std::string& name);

    const std::string& rootDir() const { return root_; }

private:
    std::string schemaPath(const std::string& name) const;
    std::string dataPath(const std::string& name) const;
    void loadExisting();

    std::string root_;
    std::map<std::string, TableDefinition> definitions_;
    std::map<std::string, std::unique_ptr<TableWrapper>> open_;
};
