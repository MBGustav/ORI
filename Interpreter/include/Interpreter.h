#pragma once

#include <string>
#include <vector>

#include "AST.h"
#include "Catalog.h"
#include "Executor.h"
#include "Result.h"

/*
 * Interpreter.h
 *
 * Front-end facade: lex + parse + execute. This is the only entry point the
 * REPL and the tests need.
 */
class Interpreter {
public:
    explicit Interpreter(std::string tableDir);

    // Parses and runs every ';'-separated statement in @p sql, returning one
    // Result per statement. Throws SqlError on the first error.
    std::vector<Result> execute(const std::string& sql);

    // Runs an already-parsed statement.
    Result execute(const Statement& statement);

    Catalog& catalog() { return catalog_; }

private:
    Catalog catalog_;
    Executor executor_;
};
