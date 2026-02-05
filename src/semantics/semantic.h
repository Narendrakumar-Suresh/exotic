#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../parser/ast.h"
#include "symbol_table.h"
#include <vector>
#include <memory>
#include <iostream>

class SemanticAnalyzer {
public:
    SemanticAnalyzer(SymbolTable& symbols) : symbols(symbols) {}

    void analyze(const std::vector<std::unique_ptr<Stmt>>& program);
    Type analyzeExpr(const Expr* expr);

private:
    SymbolTable& symbols;

    // Helper for reporting errors
    void error(const std::string& message, int line, int column);
};

#endif