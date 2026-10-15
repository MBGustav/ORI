#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "baseline_common.h"
#include "TableSchema.h"
#include "RecordStore.h"

class TableWrapper {

private:
    TableSchema schema_;
    RecordStore recordStore_;

public:
    TableWrapper(const TableSchema& schema, const std::string& filePath)
        : schema_(schema), recordStore_(filePath) {
        rebuildIndexes();
    }

    void insertRecord(const std::vector<Value>& values) {
        if (values.size() != schema_.columns.size()) {
            throw std::invalid_argument("Number of values does not match schema");
        }

        const RecordID id = recordStore_.insert(values);
        indexRecord(id, values);
    }

    std::size_t recordCount() const {
        return recordStore_.size();
    }

    // Returns the RecordID of every row whose @p columnName equals @p value,
    // using the column index (the column must be flagged `indexed`).
    std::vector<RecordID> find(
        const std::string& columnName,
        const Value& value
    ) const {
        for (std::size_t columnId = 0; columnId < schema_.columns.size(); ++columnId) {
            if (schema_.columns[columnId].name != columnName) {
                continue;
            }

            if (!schema_.columns[columnId].indexed) {
                throw std::invalid_argument(
                    "TableWrapper::find: column is not indexed: " + columnName
                );
            }

            std::vector<RecordID> ids;
            schema_.indexes[columnId].search(valueToKey(value), ids);
            return ids;
        }

        throw std::invalid_argument(
            "TableWrapper::find: unknown column: " + columnName
        );
    }

    const TableSchema& schema() const {
        return schema_;
    }

    const RecordStore& recordStore() const {
        return recordStore_;
    }

private:
    // Rebuilds the indexes from the rows already persisted in the RecordStore,
    // so a reopened table has indexes that reflect the on-disk data.
    void rebuildIndexes() {
        for (RecordID id = 0; id < recordStore_.size(); ++id) {
            const Record& record = recordStore_.at(id);
            indexRecord(id, record.values);
        }
    }

    void indexRecord(RecordID id, const std::vector<Value>& values) {
        const std::size_t columns = std::min(schema_.columns.size(), values.size());
        for (std::size_t columnId = 0; columnId < columns; ++columnId) {
            if (schema_.columns[columnId].indexed) {
                schema_.indexes[columnId].insert(valueToKey(values[columnId]), id);
            }
        }
    }
};
