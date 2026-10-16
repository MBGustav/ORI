#pragma once

#include <cstddef>
#include <string>
#include <utility>

/*
 * Token.h
 *
 * The lexical units produced by Lexer. Keywords get their own TokenType so the
 * parser can dispatch on a single switch instead of comparing strings; every
 * other identifier shares TokenType::Identifier.
 */
enum class TokenType {
    EndOfInput,

    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,

    // Keywords
    Create,
    Drop,
    Table,
    Insert,
    Into,
    Values,
    Select,
    From,
    Where,
    Indexed,
    Int,
    Varchar,

    // Punctuation / operators
    LParen,
    RParen,
    Comma,
    Semicolon,
    Star,
    Eq,
    Neq,
    Lt,
    Le,
    Gt,
    Ge
};

struct Token {
    TokenType type = TokenType::EndOfInput;
    // Original text for identifiers and keywords; the decoded value for
    // literals (quotes already removed, escape sequences resolved).
    std::string text;
    std::size_t line = 1;
    std::size_t column = 1;

    Token() = default;
    Token(TokenType tokenType, std::string tokenText, std::size_t tokenLine,
          std::size_t tokenColumn)
        : type(tokenType), text(std::move(tokenText)), line(tokenLine),
          column(tokenColumn) {}
};

// Human-readable name of a token type, used in parser/lexer error messages.
const char* tokenTypeName(TokenType type);
