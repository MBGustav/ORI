#include "Interpreter.h"

#include <string>
#include <utility>
#include <vector>

#include "Lexer.h"
#include "Parser.h"
#include "Result.h"

Interpreter::Interpreter(std::string tableDir)
    : catalog_(std::move(tableDir)), executor_(catalog_) {}

std::vector<Result> Interpreter::execute(const std::string& sql) {
    Lexer lexer(sql);
    Parser parser(lexer.tokenize());

    std::vector<Result> results;
    for (const Statement& statement : parser.parseProgram()) {
        results.push_back(executor_.execute(statement));
    }
    return results;
}

Result Interpreter::execute(const Statement& statement) {
    return executor_.execute(statement);
}
