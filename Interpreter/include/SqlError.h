#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

/*
 * SqlError.h
 *
 * The single exception type raised by the SQL front-end (lexer, parser and
 * executor). It carries the source position when one is known so the REPL can
 * point at the offending token, and a line/column of 0 means "no position".
 */
class SqlError : public std::runtime_error {
public:
    explicit SqlError(const std::string& message)
        : std::runtime_error(message), line_(0), column_(0) {}

    SqlError(const std::string& message, std::size_t line, std::size_t column)
        : std::runtime_error(message), line_(line), column_(column) {}

    std::size_t line() const { return line_; }
    std::size_t column() const { return column_; }

private:
    std::size_t line_;
    std::size_t column_;
};
