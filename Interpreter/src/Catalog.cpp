#include "Catalog.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "SqlError.h"

namespace fs = std::filesystem;

Catalog::Catalog(std::string rootDir) : root_(std::move(rootDir)) {
    fs::create_directories(root_);
    loadExisting();
}

std::string Catalog::schemaPath(const std::string& name) const {
    return (fs::path(root_) / (name + ".schema")).string();
}

std::string Catalog::dataPath(const std::string& name) const {
    return (fs::path(root_) / (name + ".db")).string();
}

void Catalog::loadExisting() {
    for (const fs::directory_entry& entry : fs::directory_iterator(root_)) {
        if (entry.path().extension() != ".schema") {
            continue;
        }
        TableDefinition definition = SchemaFile::read(entry.path().string());
        definitions_[definition.name] = std::move(definition);
    }
}

bool Catalog::exists(const std::string& name) const {
    return definitions_.count(name) != 0;
}

std::vector<std::string> Catalog::tableNames() const {
    std::vector<std::string> names;
    names.reserve(definitions_.size());
    for (const auto& pair : definitions_) {
        names.push_back(pair.first);
    }
    return names;
}

const TableDefinition& Catalog::definition(const std::string& name) const {
    const auto it = definitions_.find(name);
    if (it == definitions_.end()) {
        throw SqlError("unknown table '" + name + "'");
    }
    return it->second;
}

TableWrapper& Catalog::open(const std::string& name) {
    const auto cached = open_.find(name);
    if (cached != open_.end()) {
        return *cached->second;
    }

    const TableDefinition& def = definition(name);
    TableSchema schema(def.columns);

    auto wrapper = std::make_unique<TableWrapper>(schema, dataPath(name));
    TableWrapper* raw = wrapper.get();
    open_[name] = std::move(wrapper);
    return *raw;
}

void Catalog::create(const std::string& name,
                     const std::vector<Column>& columns) {
    if (exists(name)) {
        throw SqlError("table '" + name + "' already exists");
    }

    TableDefinition definition{name, columns};
    SchemaFile::write(schemaPath(name), definition);

    // Materialise an empty data file so the table shows up on disk at once.
    std::ofstream(dataPath(name), std::ios::binary | std::ios::trunc).close();

    definitions_[name] = std::move(definition);
}

void Catalog::drop(const std::string& name) {
    const auto it = definitions_.find(name);
    if (it == definitions_.end()) {
        throw SqlError("unknown table '" + name + "'");
    }

    // Destroy the cached wrapper first so the data file is closed before removal.
    open_.erase(name);

    std::error_code error;
    fs::remove(schemaPath(name), error);
    fs::remove(dataPath(name), error);

    definitions_.erase(it);
}
