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

    schema.indexes[0].insert("1", 7);

    std::vector<RecordID> ids;
    schema.indexes[0].search("1", ids);
    check(ids.size() == 1 && ids[0] == 7,
          "index 0 maps a value to a RecordID");

    std::vector<RecordID> independent;
    schema.indexes[1].search("1", independent);
    check(independent.empty(), "indexes are independent");

    // Copying a schema must deep-copy its indexes.
    TableSchema copy = schema;
    copy.indexes[0].insert("99", 8);

    std::vector<RecordID> copyIds;
    copy.indexes[0].search("99", copyIds);
    check(copyIds.size() == 1 && copyIds[0] == 8, "copy has the new entry");

    std::vector<RecordID> originalNew;
    schema.indexes[0].search("99", originalNew);
    check(originalNew.empty(), "original is unaffected by the copy");

    std::vector<RecordID> originalKept;
    schema.indexes[0].search("1", originalKept);
    check(originalKept.size() == 1 && originalKept[0] == 7,
          "original keeps its own entries");
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

// ------------------------------------------------- RecordStore pagination

void testRecordStorePaging() {
    std::cout << "\n== RecordStore pagination ==\n";

    const std::string path = "record_store_paging_ut.bin";
    std::remove(path.c_str());

    const int total = 100;
    const std::string filler(200, 'x');

    {
        RecordStore store(path);

        for (int i = 0; i < total; ++i) {
            store.insert({Value{int64_t{i}}, Value{filler}});
        }

        check(store.size() == static_cast<std::size_t>(total),
              "all rows are stored in memory");
        check(store.pageCount() > 1, "large batch spans multiple pages");
    }

    {
        RecordStore reopened(path);
        check(reopened.size() == static_cast<std::size_t>(total),
              "all rows persist across pages");

        bool intact = std::get<int64_t>(reopened.at(0).values[0]) == 0 &&
                      std::get<int64_t>(reopened.at(total - 1).values[0]) == total - 1;
        check(intact, "first and last rows survive the page boundary");

        check(std::get<std::string>(reopened.at(50).values[1]) == filler,
              "string value survives the page boundary");

        // Appending after a reload inserts through the B+Tree and is durable.
        reopened.insert({Value{int64_t{total}}, Value{filler}});
        check(reopened.size() == static_cast<std::size_t>(total + 1),
              "appending after reload grows the store");
    }

    {
        RecordStore reopened(path);
        check(reopened.size() == static_cast<std::size_t>(total + 1),
              "appended row persists across another reload");
        check(std::get<int64_t>(reopened.at(total).values[0]) == total,
              "appended row has the expected key");
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

    // The indexed column ("id") maps values back to their row ids.
    const std::vector<RecordID> rowOne = table.find("id", Value{int64_t{1}});
    check(rowOne.size() == 1 && rowOne[0] == 0,
          "find maps a value to its RecordID");

    const std::vector<RecordID> rowTwo = table.find("id", Value{int64_t{2}});
    check(rowTwo.size() == 1 && rowTwo[0] == 1,
          "find maps the second value");

    check(std::get<std::string>(table.recordStore().at(0).values[1]) == "alice",
          "record is readable through the store");

    // Duplicate values are supported and both rows are returned.
    table.insertRecord({Value{int64_t{1}}, Value{std::string{"alice2"}}});
    const std::vector<RecordID> duplicates = table.find("id", Value{int64_t{1}});
    check(duplicates.size() == 2, "duplicate values map to multiple rows");
    check(table.recordCount() == 3, "duplicate insert grows the record count");

    bool threw = false;
    try {
        table.insertRecord({Value{int64_t{3}}});  // too few values
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    check(threw, "insertRecord rejects a value count mismatch");
    check(table.recordCount() == 3, "rejected insert does not change the count");

    bool lookupThrew = false;
    try {
        table.find("name", Value{std::string{"alice"}});  // not indexed
    } catch (const std::invalid_argument&) {
        lookupThrew = true;
    }
    check(lookupThrew, "find rejects a non-indexed column");

    // Reopening the table must reload the rows AND rebuild the indexes.
    {
        TableWrapper reopened(schema, path);
        check(reopened.recordCount() == 3, "reopened table reloads its rows");

        const std::vector<RecordID> ids1 = reopened.find("id", Value{int64_t{1}});
        check(ids1.size() == 2,
              "index is rebuilt on reopen (duplicate value)");

        const std::vector<RecordID> ids2 = reopened.find("id", Value{int64_t{2}});
        check(ids2.size() == 1 && ids2[0] == 1,
              "index is rebuilt on reopen (second value)");
    }

    std::remove(path.c_str());
}

// --------------------------------------------------------------- IndexTree

void testIndexTree() {
    std::cout << "\n== IndexTree ==\n";

    IndexTree index;
    check(index.empty(), "a new index is empty");

    check(index.insert("alice", 0), "insert (value, id)");
    check(index.insert("alice", 5), "insert a duplicate value with a new id");
    check(!index.insert("alice", 5), "the same (value, id) pair is rejected");
    check(index.insert("bob", 1), "insert a different value");

    std::vector<RecordID> alice;
    check(index.search("alice", alice) == 2, "search returns every matching row");
    check(alice.size() == 2 && alice[0] == 0 && alice[1] == 5,
          "matching RecordIDs are returned in order");

    std::vector<RecordID> bob;
    check(index.search("bob", bob) == 1 && bob[0] == 1,
          "lookup is scoped to the exact value");

    std::vector<RecordID> none;
    check(index.search("carol", none) == 0, "search misses an absent value");

    check(index.contains("alice"), "contains finds a stored value");
    check(!index.contains("carol"), "contains misses an absent value");

    check(index.remove("alice", 0), "remove a (value, id) pair");
    std::vector<RecordID> afterRemove;
    check(index.search("alice", afterRemove) == 1 && afterRemove[0] == 5,
          "only the removed row is gone");
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
    testRecordStorePaging();
    testTableWrapper();
    testIndexTree();
    testBinPlusTreeCopy();

    std::cout << "\n";
    if (failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }

    std::cout << failures << " check(s) failed.\n";
    return 1;
}
