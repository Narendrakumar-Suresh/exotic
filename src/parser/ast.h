#ifndef AST_H
#define AST_H

#include <string>
#include "../semantics/types.h"
using namespace std;

struct Expr{
    int value;
    Type type;
};

struct LetStmt {
    string name;
    Expr expr;
};

#endif