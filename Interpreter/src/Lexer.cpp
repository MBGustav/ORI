#include "Lexer.h"

#include <cctype>
#include <cstddef>
#include <string>
#include <unordered_map>

#include "SqlError.h"
#include "Token.h"

namespace {

const std::unordered_map<std::string, TokenType>& keywordTable() {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"CREATE", TokenType::Create},
        {"DROP", TokenType::Drop},
        {"TABLE", TokenType::Table},
        {"INSERT", TokenType::Insert},
        {"INTO", TokenType::Into},
        {"VALUES", TokenType::Values},
        {"SELECT", TokenType::Select},
        {"FROM", TokenType::From},
        {"WHERE", TokenType::Where},
        {"INDEXED", TokenType::Indexed},
        {"INT", TokenType::Int},
        {"INTEGER", TokenType::Int},
        {"VARCHAR", TokenType::Varchar},
        {"STRING", TokenType::Varchar},
        {"TEXT", TokenType::Varchar},
    };
    return keywords;
}

std::string toUpperAscii(std::string value) {
    for (char& character : value) {
        character = static_cast<char>(
            std::toupper(static_cast<unsigned char>(character)));
    }
    return value;
}

}  // namespace

const char* tokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::EndOfInput: return "end of input";
        case TokenType::Identifier: return "identifier";
        case TokenType::IntegerLiteral: return "integer";
        case TokenType::FloatLiteral: return "float";
        case TokenType::StringLiteral: return "string";
        case TokenType::Create: return "CREATE";
        case TokenType::Drop: return "DROP";
        case TokenType::Table: return "TABLE";
        case TokenType::Insert: return "INSERT";
        case TokenType::Into: return "INTO";
        case TokenType::Values: return "VALUES";
        case TokenType::Select: return "SELECT";
        case TokenType::From: return "FROM";
        case TokenType::Where: return "WHERE";
        case TokenType::Indexed: return "INDEXED";
        case TokenType::Int: return "INT";
        case TokenType::Varchar: return "VARCHAR";
        case TokenType::LParen: return "'('";
        case TokenType::RParen: return "')'";
        case TokenType::Comma: return "','";
        case TokenType::Semicolon: return "';'";
        case TokenType::Star: return "'*'";
        case TokenType::Eq: return "'='";
        case TokenType::Neq: return "'<>'";
        case TokenType::Lt: return "'<'";
        case TokenType::Le: return "'<='";
        case TokenType::Gt: return "'>'";
        case TokenType::Ge: return "'>='";
    }
    return "token";
}

char Lexer::peek(std::size_t offset) const {
    const std::size_t index = pos_ + offset;
    if (index >= source_.size()) {
        return '\0';
    }
    return source_[index];
}

// Advances the current position and returns the character at the previous
// position. Updates line and column numbers.  
char Lexer::advance() {
    const char character = source_[pos_++];
    if (character == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return character;
}

bool Lexer::atEnd() const {
    return pos_ >= source_.size();
}

void Lexer::skipWhitespaceAndComments() {
    while (!atEnd()) {
        const char character = peek();
        if (std::isspace(static_cast<unsigned char>(character))) {
            advance();
            continue;
        }
        // Line comment: "-- ..." up to the end of line.
        if (character == '-' && peek(1) == '-') {
            while (!atEnd() && peek() != '\n') {
                advance();
            }
            continue;
        }
        break;
    }
}

Token Lexer::identifierOrKeyword() {
    const std::size_t startLine = line_;
    const std::size_t startColumn = column_;
    std::string text;

    while (!atEnd()) {
        const char character = peek();
        if (std::isalnum(static_cast<unsigned char>(character)) ||
            character == '_') {
            text += advance();
        } else {
            break;
        }
    }

    const auto it = keywordTable().find(toUpperAscii(text));
    if (it != keywordTable().end()) {
        return Token(it->second, text, startLine, startColumn);
    }
    return Token(TokenType::Identifier, text, startLine, startColumn);
}

Token Lexer::numberLiteral() {
    const std::size_t startLine = line_;
    const std::size_t startColumn = column_;
    std::string text;
    bool isFloat = false;

    while (!atEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        text += advance();
    }
    if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peek(1)))) {
        isFloat = true;
        text += advance();  // '.'
        while (!atEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
            text += advance();
        }
    }

    return Token(isFloat ? TokenType::FloatLiteral : TokenType::IntegerLiteral,
                 text, startLine, startColumn);
}

Token Lexer::stringLiteral() {
    const std::size_t startLine = line_;
    const std::size_t startColumn = column_;
    advance();  // opening quote

    std::string value;
    while (true) {
        if (atEnd()) {
            throw SqlError("unterminated string literal", startLine, startColumn);
        }
        const char character = advance();
        if (character == '\'') {
            if (peek() == '\'') {  // '' escape -> a single quote
                advance();
                value += '\'';
                continue;
            }
            break;
        }
        value += character;
    }

    return Token(TokenType::StringLiteral, value, startLine, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipWhitespaceAndComments();
        if (atEnd()) {
            tokens.emplace_back(TokenType::EndOfInput, "", line_, column_);
            break;
        }

        const char character = peek();
        const std::size_t startLine = line_;
        const std::size_t startColumn = column_;

        if (std::isalpha(static_cast<unsigned char>(character)) ||
            character == '_') {
            tokens.push_back(identifierOrKeyword());
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(character))) {
            tokens.push_back(numberLiteral());
            continue;
        }
        if (character == '\'') {
            tokens.push_back(stringLiteral());
            continue;
        }

        switch (character) {
            case '(':
                advance();
                tokens.emplace_back(TokenType::LParen, "(", startLine, startColumn);
                break;
            case ')':
                advance();
                tokens.emplace_back(TokenType::RParen, ")", startLine, startColumn);
                break;
            case ',':
                advance();
                tokens.emplace_back(TokenType::Comma, ",", startLine, startColumn);
                break;
            case ';':
                advance();
                tokens.emplace_back(TokenType::Semicolon, ";", startLine, startColumn);
                break;
            case '*':
                advance();
                tokens.emplace_back(TokenType::Star, "*", startLine, startColumn);
                break;
            case '=':
                advance();
                tokens.emplace_back(TokenType::Eq, "=", startLine, startColumn);
                break;
            case '<':
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.emplace_back(TokenType::Le, "<=", startLine, startColumn);
                } else if (peek() == '>') {
                    advance();
                    tokens.emplace_back(TokenType::Neq, "<>", startLine, startColumn);
                } else {
                    tokens.emplace_back(TokenType::Lt, "<", startLine, startColumn);
                }
                break;
            case '>':
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.emplace_back(TokenType::Ge, ">=", startLine, startColumn);
                } else {
                    tokens.emplace_back(TokenType::Gt, ">", startLine, startColumn);
                }
                break;
            case '!':
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.emplace_back(TokenType::Neq, "!=", startLine, startColumn);
                } else {
                    throw SqlError("unexpected character '!'", startLine, startColumn);
                }
                break;
            default: {
                std::string message = "unexpected character '";
                message += character;
                message += "'";
                throw SqlError(message, startLine, startColumn);
            }
        }
    }

    return tokens;
}
