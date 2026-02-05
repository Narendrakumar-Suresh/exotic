#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include "../semantics/types.h"
using namespace std;

enum class ExprKind {
    NumberLiteral,
    StringLiteral,
    Identifier,
    Binary,
    Unary,
    StringSlice,
    MethodCall,
    ListLiteral
};

struct Expr {
    ExprKind kind;
    Type type;
    
    int int_value;
    double double_value;
    string string_value;
    
    string op;
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;
    
    unique_ptr<Expr> start;
    unique_ptr<Expr> end;
    
    string method_name;
    vector<unique_ptr<Expr>> args;
    vector<unique_ptr<Expr>> elements;

    int line;
    int column;
    
    Expr() : kind(ExprKind::NumberLiteral), int_value(0), double_value(0.0), line(0), column(0) {}
    Expr(int l, int c) : kind(ExprKind::NumberLiteral), int_value(0), double_value(0.0), line(l), column(c) {}
};

struct Stmt {
    virtual ~Stmt() = default;
};

struct LetStmt : public Stmt {
    string name;
    Type declared_type;
    unique_ptr<Expr> expr;

    LetStmt() : declared_type(unknownType()) {}
};

struct PrintStmt : public Stmt {
    unique_ptr<Expr> expr;
};

#endif