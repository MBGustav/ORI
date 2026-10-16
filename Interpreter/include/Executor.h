#pragma once

#include "AST.h"
#include "Catalog.h"
#include "Result.h"

/*
 * Executor.h
 *
 * Walks an AST statement and drives the Catalog / TableWrapper. All errors are
 * reported as SqlError (unknown table/column, wrong arity, type mismatch, ...).
 */
class Executor {
public:
    explicit Executor(Catalog& catalog) : catalog_(catalog) {}

    Result execute(const Statement& statement);

private:
    Result executeCreate(const CreateTableStmt& statement);
    Result executeDrop(const DropTableStmt& statement);
    Result executeInsert(const InsertStmt& statement);
    Result executeSelect(const SelectStmt& statement);

    Catalog& catalog_;
};
