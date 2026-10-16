#pragma once

#include <string>
#include <vector>

#include "Token.h"

/*
 * Lexer.h
 *
 * Converts a SQL string into a flat list of Tokens. Keywords are recognised
 * case-insensitively; identifiers keep their original spelling. String literals
 * use single quotes and support the '' escape for an embedded quote.
 */
class Lexer {
public:
    explicit Lexer(std::string source) : source_(std::move(source)) {}

    // Scans the whole input. Throws SqlError on an illegal character or an
    // unterminated string literal. Always ends with a TokenType::EndOfInput.
    std::vector<Token> tokenize();

private:
    char peek(std::size_t offset = 0) const;
    char advance();
    bool atEnd() const;

    void skipWhitespaceAndComments();
    Token identifierOrKeyword();
    Token numberLiteral();
    Token stringLiteral();

    std::string source_;
    std::size_t pos_ = 0;
    std::size_t line_ = 1;
    std::size_t column_ = 1;
};
