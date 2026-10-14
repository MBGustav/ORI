#pragma once

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
        : schema_(schema), recordStore_(filePath) {}

    void insertRecord(const std::vector<Value>& values) {
        if (values.size() != schema_.columns.size()) {
            throw std::invalid_argument("Number of values does not match schema");
        }

        recordStore_.insert(values);

        for (std::size_t columnId = 0; columnId < schema_.columns.size(); ++columnId) {
            if (!schema_.columns[columnId].indexed) {
                continue;
            }
            schema_.indexes[columnId].insertNode(valueToKey(values[columnId]));
        }
    }

    std::size_t recordCount() const {
        return recordStore_.size();
    }

    const TableSchema& schema() const {
        return schema_;
    }

    const RecordStore& recordStore() const {
        return recordStore_;
    }
};
