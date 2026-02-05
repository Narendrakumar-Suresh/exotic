#ifndef AST_H
#define AST_H
#include <string>
using namespace std;

struct Expr{
    Type type;
    int int_value;
    double float_value;
    string string_value;

};

struct LetStmt {
    string name;
    Expr expr;
};


#endif
