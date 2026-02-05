#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "../parser/ast.h"
#include "../semantics/symbol_table.h"
#include <string>
#include <memory>

class Evaluator {
public:
    Evaluator(SymbolTable& symbols);
    
    void evalProgram(const std::vector<std::unique_ptr<Stmt>>& program);
    Symbol evalExpr(const Expr* expr);
    void printSymbol(const Symbol& value);
    
private:
    SymbolTable& symbols;
    
    Symbol evalBinary(const Expr* expr);
    Symbol evalUnary(const Expr* expr);
    Symbol evalStringSlice(const Expr* expr);
    Symbol evalMethodCall(const Expr* expr);
    void evalPrintStmt(const PrintStmt* stmt);
};

#endif