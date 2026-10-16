#include "Parser.h"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "AST.h"
#include "SqlError.h"
#include "Token.h"

namespace {

// Builds a positioned error message for the token that stopped the parse.
std::string tokenDescription(const Token& token) {
    if (token.type == TokenType::EndOfInput) {
        return "end of input";
    }
    return std::string(tokenTypeName(token.type)) + " '" + token.text + "'";
}

}  // namespace

const Token& Parser::peek(std::size_t offset) const {
    const std::size_t target = index_ + offset;
    if (target >= tokens_.size()) {
        return tokens_.back();  // EndOfInput sentinel
    }
    return tokens_[target];
}

const Token& Parser::current() const {
    return peek(0);
}

bool Parser::atEnd() const {
    return current().type == TokenType::EndOfInput;
}

const Token& Parser::advance() {
    const Token& token = current();
    if (!atEnd()) {
        ++index_;
    }
    return token;
}

bool Parser::check(TokenType type) const {
    return current().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::expect(TokenType type, const std::string& what) {
    if (!check(type)) {
        throw SqlError(
            "expected " + what + ", found " + tokenDescription(current()),
            current().line, current().column);
    }
    return advance();
}

void Parser::skipSeparators() {
    while (match(TokenType::Semicolon)) {
    }
}

std::vector<Statement> Parser::parseProgram() {
    std::vector<Statement> statements;
    skipSeparators();

    while (!atEnd()) {
        statements.push_back(parseStatement());
        if (!atEnd() && !check(TokenType::Semicolon)) {
            throw SqlError(
                "expected ';' between statements, found " +
                    tokenDescription(current()),
                current().line, current().column);
        }
        skipSeparators();
    }

    return statements;
}

Statement Parser::parseStatement() {
    switch (current().type) {
        case TokenType::Create: return parseCreate();
        case TokenType::Drop: return parseDrop();
        case TokenType::Insert: return parseInsert();
        case TokenType::Select: return parseSelect();
        default:
            throw SqlError(
                "expected a statement (CREATE, DROP, INSERT or SELECT), found " +
                    tokenDescription(current()),
                current().line, current().column);
    }
}

std::string Parser::expectIdentifier(const std::string& what) {
    const Token& token = expect(TokenType::Identifier, what);
    return token.text;
}

Statement Parser::parseCreate() {
    advance();  // CREATE
    expect(TokenType::Table, "TABLE");

    CreateTableStmt statement;
    statement.table = expectIdentifier("table name");

    expect(TokenType::LParen, "'('");
    statement.columns.push_back(parseColumnDef());
    while (match(TokenType::Comma)) {
        statement.columns.push_back(parseColumnDef());
    }
    expect(TokenType::RParen, "')'");

    return statement;
}

ColumnDef Parser::parseColumnDef() {
    const std::string name = expectIdentifier("column name");
    const DataType type = parseType();
    const bool indexed = match(TokenType::Indexed);
    return ColumnDef(name, type, indexed);
}

DataType Parser::parseType() {
    if (match(TokenType::Int)) {
        return DataType::Integer;
    }
    if (match(TokenType::Varchar)) {
        return DataType::Varchar;
    }
    throw SqlError(
        "expected a data type (INT or VARCHAR), found " +
            tokenDescription(current()),
        current().line, current().column);
}

Statement Parser::parseDrop() {
    advance();  // DROP
    expect(TokenType::Table, "TABLE");

    DropTableStmt statement;
    statement.table = expectIdentifier("table name");
    return statement;
}

Statement Parser::parseInsert() {
    advance();  // INSERT
    expect(TokenType::Into, "INTO");

    InsertStmt statement;
    statement.table = expectIdentifier("table name");

    expect(TokenType::Values, "VALUES");
    expect(TokenType::LParen, "'('");
    statement.values.push_back(parseLiteral());
    while (match(TokenType::Comma)) {
        statement.values.push_back(parseLiteral());
    }
    expect(TokenType::RParen, "')'");

    return statement;
}

Statement Parser::parseSelect() {
    advance();  // SELECT

    SelectStmt statement;
    if (match(TokenType::Star)) {
        statement.selectAll = true;
    } else {
        statement.selectAll = false;
        statement.columns.push_back(expectIdentifier("column name"));
        while (match(TokenType::Comma)) {
            statement.columns.push_back(expectIdentifier("column name"));
        }
    }

    expect(TokenType::From, "FROM");
    statement.table = expectIdentifier("table name");

    if (match(TokenType::Where)) {
        statement.hasWhere = true;
        statement.where.column = expectIdentifier("column name");
        statement.where.op = parseComparisonOp();
        statement.where.value = parseLiteral();
    }

    return statement;
}

ComparisonOp Parser::parseComparisonOp() {
    switch (current().type) {
        case TokenType::Eq: advance(); return ComparisonOp::Eq;
        case TokenType::Neq: advance(); return ComparisonOp::Neq;
        case TokenType::Lt: advance(); return ComparisonOp::Lt;
        case TokenType::Le: advance(); return ComparisonOp::Le;
        case TokenType::Gt: advance(); return ComparisonOp::Gt;
        case TokenType::Ge: advance(); return ComparisonOp::Ge;
        default:
            throw SqlError(
                "expected a comparison operator (=, <>, <, <=, >, >=), found " +
                    tokenDescription(current()),
                current().line, current().column);
    }
}

Value Parser::parseLiteral() {
    const Token& token = current();
    switch (token.type) {
        case TokenType::IntegerLiteral:
            advance();
            return Value{static_cast<int64_t>(std::stoll(token.text))};
        case TokenType::FloatLiteral:
            advance();
            return Value{std::stod(token.text)};
        case TokenType::StringLiteral:
            advance();
            return Value{token.text};
        default:
            throw SqlError(
                "expected a literal value, found " + tokenDescription(token),
                token.line, token.column);
    }
}
