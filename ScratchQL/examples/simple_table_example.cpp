// Simple end-to-end example of the table layer.
//
// It creates a table with the columns
//   (id INTEGER indexed, nome VARCHAR, idade INTEGER indexed, salario INTEGER)
// inserts 100 rows, and then searches for all rows with a given "idade".
//
// Notes:
//   * find() only supports equality on a column flagged `indexed`. Here only
//     "id" and "idade" are indexed; "nome" and "salario" are not.
//   * Several rows can share the same "idade"; the index returns every matching
//     RecordID (duplicate values are supported).
//   * The DataType enum has no Float, so "salario" is labelled Integer while
//     its value is stored as a double. DataType is metadata only; the stored
//     Value is what is persisted.
//
// How to build/run (from the repository root):
//   g++ -std=c++17 -I ScratchQL/include \
//       ScratchQL/examples/simple_table_example.cpp ScratchQL/src/BinPlusTree.cpp \
//       -o /tmp/simple_table_example && /tmp/simple_table_example
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "baseline_common.h"
#include "TableSchema.h"
#include "TableWrapper.h"

namespace {

std::string valueToString(const Value& value) {
    switch (value.index()) {
        case 0: return std::to_string(std::get<int64_t>(value));
        case 1: return std::to_string(std::get<double>(value));
        case 2: return std::get<bool>(value) ? "true" : "false";
        case 3: return std::get<std::string>(value);
        default: return {};
    }
}

void printRow(const Record& record) {
    std::cout << "  id=" << valueToString(record.values[0])
              << " nome=" << valueToString(record.values[1])
              << " idade=" << valueToString(record.values[2])
              << " salario=" << valueToString(record.values[3])
              << "\n";
}

void searchByAge(const TableWrapper& table, int64_t age) {
    const std::vector<RecordID> ids = table.find("idade", Value{age});
    std::cout << "Pesquisa por idade = " << age
              << " -> " << ids.size() << " registro(s)\n";
    for (const RecordID id : ids) {
        printRow(table.recordStore().at(id));
    }
}

}  // namespace

int main() {
    const std::string path = "people.db";
    std::remove(path.c_str());

    const std::vector<Column> columns = {
        Column{"id", DataType::Integer, true},
        Column{"nome", DataType::Varchar, false},
        Column{"idade", DataType::Integer, true},
        Column{"salario", DataType::Integer, false},
    };

    TableSchema schema(columns);
    TableWrapper table(schema, path);

    for (int i = 0; i < 100; ++i) {
        const int64_t idade = 18 + (i % 50);          // 18..67, so ages repeat
        const double salario = 3000.0 + i * 37.5;
        table.insertRecord({
            Value{static_cast<int64_t>(i + 1)},
            Value{std::string{"Pessoa " + std::to_string(i + 1)}},
            Value{idade},
            Value{salario},
        });
    }

    // std::cout << "Registros na tabela: " << table.recordCount() << "\n\n";
    std::cout << "Metadados da tabela:\n";
    table.show_metadata();


    std::cout << "== Tabela aberta ==\n";
    searchByAge(table, 30);

    // Reopening rebuilds the indexes from the persisted rows.
    std::cout << "\n== Tabela reaberta do disco ==\n";
    TableWrapper reopened(schema, path);
    std::cout << "Registros na tabela: " << reopened.recordCount() << "\n";
    searchByAge(reopened, 30);

    // std::remove(path.c_str());
    return 0;
}
