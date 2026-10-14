// Simple unit tests for the table layer: TableSchema/Indexes, RecordStore,
// TableWrapper and BinPlusTree copy semantics.
//
// How to build/run (from the repository root):
//   g++ -std=c++17 -I ScratchQL/include \
//       ScratchQL/unit_tests/table_ut.cpp ScratchQL/src/BinPlusTree.cpp \
//       -o /tmp/table_ut && /tmp/table_ut
//
// The program prints [PASSED]/[FAILED] per check and returns non-zero if any
// check fails.
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "baseline_common.h"
#include "BinPlusTree.h"
#include "TableSchema.h"
#include "RecordStore.h"
#include "TableWrapper.h"

namespace {

int failures = 0;

void check(bool condition, const std::string& label) {
    if (condition) {
        std::cout << "[PASSED] " << label << "\n";
    } else {
        ++failures;
        std::cout << "[FAILED] " << label << "\n";
    }
}

// ------------------------------------------------------------- TableSchema

void testTableSchema() {
    std::cout << "\n== TableSchema ==\n";

    check(valueToKey(Value{int64_t{42}}) == "42", "valueToKey(int64)");
    check(valueToKey(Value{double{3.5}}) == "3.500000", "valueToKey(double)");
    check(valueToKey(Value{true}) == "1", "valueToKey(bool true)");
    check(valueToKey(Value{false}) == "0", "valueToKey(bool false)");
    check(valueToKey(Value{std::string{"alice"}}) == "alice", "valueToKey(string)");

    const std::vector<Column> columns = {
        Column{"id", DataType::Integer, true},
        Column{"name", DataType::Varchar, false},
        Column{"email", DataType::Varchar, true},
    };

    TableSchema schema(columns);

    check(schema.columnCount() == 3, "schema records the column count");
    check(schema.indexes.size() == 3, "one index per column");

    schema.indexes[0].insertNode("1");
    check(schema.indexes[0].searchNode("1"), "index 0 stores a key");
    check(!schema.indexes[1].searchNode("1"), "indexes are independent");

    // Copying a schema must deep-copy its indexes.
    TableSchema copy = schema;
    copy.indexes[0].insertNode("99");

    check(copy.indexes[0].searchNode("99"), "copy has the new key");
    check(!schema.indexes[0].searchNode("99"), "original is unaffected by the copy");
    check(schema.indexes[0].searchNode("1"), "original keeps its own keys");
}

// -------------------------------------------------------------- RecordStore

void testRecordStore() {
    std::cout << "\n== RecordStore ==\n";

    const std::string path = "record_store_ut.bin";
    std::remove(path.c_str());

    {
        RecordStore store(path);
        check(store.empty(), "a fresh store is empty");
        check(store.size() == 0, "size is zero when empty");

        const RecordID first = store.insert({
            Value{int64_t{1}}, Value{std::string{"alice"}}, Value{true}
        });
        const RecordID second = store.insert({
            Value{int64_t{2}}, Value{std::string{"bob"}}, Value{false}
        });

        check(first == 0, "first RecordID is 0");
        check(second == 1, "second RecordID is 1");
        check(store.size() == 2, "size grows with each insert");

        const Record& record = store.at(first);
        check(record.values.size() == 3, "record keeps its value count");
        check(std::get<int64_t>(record.values[0]) == 1, "int64 value round-trips");
        check(std::get<std::string>(record.values[1]) == "alice", "string value round-trips");
        check(std::get<bool>(record.values[2]) == true, "bool value round-trips");

        bool threw = false;
        try {
            store.at(99);
        } catch (const std::exception&) {
            threw = true;
        }
        check(threw, "at() throws for an unknown RecordID");
    }

    {
        // Reopening the store must reload the persisted records.
        RecordStore reopened(path);
        check(reopened.size() == 2, "records persist across reopen");
        check(std::get<std::string>(reopened.at(1).values[1]) == "bob",
              "persisted value survives reload");
    }

    std::remove(path.c_str());
}

// ------------------------------------------------------------- TableWrapper

void testTableWrapper() {
    std::cout << "\n== TableWrapper ==\n";

    const std::string path = "table_wrapper_ut.bin";
    std::remove(path.c_str());

    const std::vector<Column> columns = {
        Column{"id", DataType::Integer, true},
        Column{"name", DataType::Varchar, false},
    };
    TableSchema schema(columns);

    TableWrapper table(schema, path);

    check(table.recordCount() == 0, "a new table has no records");

    table.insertRecord({Value{int64_t{1}}, Value{std::string{"alice"}}});
    table.insertRecord({Value{int64_t{2}}, Value{std::string{"bob"}}});

    check(table.recordCount() == 2, "insertRecord updates the record count");

    // The indexed column ("id") is maintained; the plain column ("name") is not.
    check(table.schema().indexes[0].searchNode("1"), "indexed column is indexed");
    check(table.schema().indexes[0].searchNode("2"), "second key is indexed");
    check(!table.schema().indexes[1].searchNode("alice"),
          "non-indexed column is not indexed");

    check(std::get<std::string>(table.recordStore().at(0).values[1]) == "alice",
          "record is readable through the store");

    bool threw = false;
    try {
        table.insertRecord({Value{int64_t{3}}});  // too few values
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    check(threw, "insertRecord rejects a value count mismatch");
    check(table.recordCount() == 2, "rejected insert does not change the count");

    std::remove(path.c_str());
}

// -------------------------------------------------- BinPlusTree copy/assign

void testBinPlusTreeCopy() {
    std::cout << "\n== BinPlusTree copy semantics ==\n";

    BinPlusTree original;
    for (int i = 0; i < 30; ++i) {
        original.insertNode("copy-" + std::to_string(i));
    }

    BinPlusTree copy(original);
    check(copy.searchNode("copy-0"), "copy contains the original keys");
    check(copy.searchNode("copy-29"), "copy contains the deepest key");

    copy.insertNode("copy-only");
    check(copy.searchNode("copy-only"), "mutating the copy adds a key");
    check(!original.searchNode("copy-only"), "original is unaffected by the copy");

    copy.deleteNode("copy-0");
    check(!copy.searchNode("copy-0"), "mutating the copy removes a key");
    check(original.searchNode("copy-0"), "original keeps the removed key");

    BinPlusTree assigned;
    assigned.insertNode("stale");
    assigned = original;
    check(assigned.searchNode("copy-10"), "assignment replaces the target");
    check(!assigned.searchNode("stale"), "assignment drops the old contents");

    assigned.insertNode("assign-only");
    check(!original.searchNode("assign-only"), "assigned tree is independent");
}

}  // namespace

int main() {
    testTableSchema();
    testRecordStore();
    testTableWrapper();
    testBinPlusTreeCopy();

    std::cout << "\n";
    if (failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }

    std::cout << failures << " check(s) failed.\n";
    return 1;
}
