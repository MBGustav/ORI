// Unit tests for the ScratchQL interpreter front-end.
//
// How to build/run (from the repository root):
//   g++ -std=c++17 -I Interpreter/include -I ScratchQL/include \
//       Interpreter/unit_tests/interpreter_ut.cpp \
//       Interpreter/src/Lexer.cpp Interpreter/src/Parser.cpp \
//       Interpreter/src/SchemaFile.cpp Interpreter/src/Catalog.cpp \
//       Interpreter/src/Executor.cpp Interpreter/src/Interpreter.cpp \
//       ScratchQL/src/BinPlusTree.cpp \
//       -o /tmp/interpreter_ut && /tmp/interpreter_ut
//
// The program prints [PASSED]/[FAILED] per check and returns non-zero if any
// check fails.
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "Interpreter.h"
#include "Result.h"
#include "SqlError.h"
#include "baseline_common.h"

namespace fs = std::filesystem;

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

bool threwSqlError(Interpreter& interpreter, const std::string& sql) {
    try {
        interpreter.execute(sql);
    } catch (const SqlError&) {
        return true;
    } catch (const std::exception&) {
        return false;
    }
    return false;
}

const fs::path kTableDir = fs::temp_directory_path() / "scratchql_interpreter_ut";

void resetDir() {
    fs::remove_all(kTableDir);
}

// -------------------------------------------------------------- lexer/parser

void testParseErrors(Interpreter& interpreter) {
    std::cout << "\n== parse errors ==\n";

    check(threwSqlError(interpreter, "SELEC * FROM t;"),
          "typo'd keyword is rejected");
    check(threwSqlError(interpreter, "CREATE TABLE;"),
          "missing table name is rejected");
    check(threwSqlError(interpreter, "CREATE TABLE t (id);"),
          "missing column type is rejected");
    check(threwSqlError(interpreter, "INSERT INTO t VALUES ('abc);"),
          "unterminated string is rejected");
    check(threwSqlError(interpreter, "SELECT * FROM t WHERE id =;"),
          "missing WHERE literal is rejected");
    check(threwSqlError(interpreter, "SELECT * FROM t WHERE id @ 1;"),
          "bad operator is rejected");
}

// ------------------------------------------------------------- happy path

void testCreateInsertSelect(Interpreter& interpreter) {
    std::cout << "\n== create / insert / select ==\n";

    interpreter.execute("CREATE TABLE people (id INT INDEXED, nome VARCHAR, idade INT INDEXED);");

    interpreter.execute(
        "INSERT INTO people VALUES (1, 'alice', 30);"
        "INSERT INTO people VALUES (2, 'bob', 25);"
        "INSERT INTO people VALUES (3, 'carol', 30);");

    const std::vector<Result> all =
        interpreter.execute("SELECT * FROM people;");
    check(all.size() == 1, "one result per statement");
    check(all[0].columns.size() == 3, "SELECT * projects every column");
    check(all[0].columns[0] == "id" && all[0].columns[2] == "idade",
          "SELECT * uses the schema column names");
    check(all[0].rows.size() == 3, "SELECT * returns every row");

    // Equality on an indexed column goes through the index.
    const std::vector<Result> byId =
        interpreter.execute("SELECT nome FROM people WHERE id = 2;");
    check(byId[0].columns.size() == 1 && byId[0].columns[0] == "nome",
          "projection keeps only the selected column");
    check(byId[0].rows.size() == 1, "index lookup returns the matching row");
    check(std::get<std::string>(byId[0].rows[0][0]) == "bob",
          "index lookup returns the right row");

    // Duplicate values on an indexed column return several rows.
    const std::vector<Result> byAge =
        interpreter.execute("SELECT id, nome FROM people WHERE idade = 30;");
    check(byAge[0].rows.size() == 2, "duplicate index values return both rows");

    // Non-indexed column forces a full scan.
    const std::vector<Result> byName =
        interpreter.execute("SELECT id FROM people WHERE nome = 'carol';");
    check(byName[0].rows.size() == 1 &&
              std::get<int64_t>(byName[0].rows[0][0]) == 3,
          "scan finds a row by a non-indexed column");

    // Range comparison (never uses the index) on an indexed column.
    const std::vector<Result> ranged =
        interpreter.execute("SELECT id FROM people WHERE idade > 28;");
    check(ranged[0].rows.size() == 2, "range predicate scans and matches");
}

// --------------------------------------------------------------- type errors

void testTypeErrors(Interpreter& interpreter) {
    std::cout << "\n== type / arity errors ==\n";

    check(threwSqlError(interpreter, "INSERT INTO people VALUES (4, 'dave');"),
          "wrong value count is rejected");
    check(threwSqlError(interpreter, "INSERT INTO people VALUES ('x', 'dave', 1);"),
          "string into an integer column is rejected");

    // The failed inserts must not have changed the table.
    const std::vector<Result> all =
        interpreter.execute("SELECT * FROM people;");
    check(all[0].rows.size() == 3, "rejected inserts leave the table unchanged");

    check(threwSqlError(interpreter, "SELECT nope FROM people;"),
          "unknown projected column is rejected");
    check(threwSqlError(interpreter, "SELECT * FROM ghost;"),
          "unknown table is rejected");
    check(threwSqlError(interpreter, "CREATE TABLE people (id INT);"),
          "creating an existing table is rejected");
}

// --------------------------------------------------------------- persistence

void testPersistence() {
    std::cout << "\n== persistence ==\n";

    {
        Interpreter interpreter(kTableDir.string());
        interpreter.execute(
            "INSERT INTO people VALUES (4, 'dave', 40);");
    }

    // A brand new interpreter on the same directory must see the rows.
    Interpreter reopened(kTableDir.string());
    const std::vector<Result> all =
        reopened.execute("SELECT id, nome FROM people WHERE id = 4;");
    check(all[0].rows.size() == 1, "rows persist across interpreter restart");
    check(std::get<std::string>(all[0].rows[0][1]) == "dave",
          "persisted row has the expected values");

    const std::vector<std::string> names = reopened.catalog().tableNames();
    check(names.size() == 1 && names[0] == "people",
          "catalog lists the persisted table");
}

// --------------------------------------------------------------------- drop

void testDrop() {
    std::cout << "\n== drop ==\n";

    Interpreter interpreter(kTableDir.string());
    interpreter.execute("DROP TABLE people;");

    check(!interpreter.catalog().exists("people"), "table is gone from the catalog");
    check(!fs::exists(kTableDir / "people.schema"), "schema file is removed");
    check(!fs::exists(kTableDir / "people.db"), "data file is removed");
    check(threwSqlError(interpreter, "SELECT * FROM people;"),
          "selecting a dropped table is rejected");
    check(threwSqlError(interpreter, "DROP TABLE people;"),
          "dropping a missing table is rejected");
}

}  // namespace

int main() {
    resetDir();

    try {
        Interpreter interpreter(kTableDir.string());
        testParseErrors(interpreter);
        testCreateInsertSelect(interpreter);
        testTypeErrors(interpreter);
        testPersistence();
        testDrop();
    } catch (const std::exception& error) {
        std::cout << "[FAILED] unexpected exception: " << error.what() << "\n";
        ++failures;
    }

    resetDir();

    std::cout << "\n";
    if (failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }
    std::cout << failures << " check(s) failed.\n";
    return 1;
}
