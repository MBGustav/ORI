#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "AST.h"
#include "Token.h"

/*
 * Parser.h
 *
 * Hand-written recursive-descent parser over the token stream produced by
 * Lexer. parseProgram() accepts one or more ';'-separated statements and
 * returns them in order; parseStatement() consumes exactly one.
 */
class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

    // Parses every statement up to EndOfInput. Statements must be separated by
    // ';'; a trailing statement may omit the final ';'.
    std::vector<Statement> parseProgram();

    // Parses a single statement (used by tests).
    Statement parseStatement();

private:
    const Token& peek(std::size_t offset = 0) const;
    const Token& current() const;
    bool atEnd() const;
    const Token& advance();

    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& expect(TokenType type, const std::string& what);

    void skipSeparators();

    Statement parseCreate();
    Statement parseDrop();
    Statement parseInsert();
    Statement parseSelect();

    ColumnDef parseColumnDef();
    DataType parseType();
    Value parseLiteral();
    ComparisonOp parseComparisonOp();

    std::string expectIdentifier(const std::string& what);

    std::vector<Token> tokens_;
    std::size_t index_ = 0;
};
