#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "ast.h"
#include "../semantics/symbol_table.h"
#include <vector>

class Parser {
public:
    Parser(Lexer& lexer);
    LetStmt parseLet();
    std::vector<LetStmt> parseProgram();

private:
    Lexer& lexer;
    Token current;
    SymbolTable symbols;
    Type parseType();

    void advance();
    void expect(TokenKind kind);
    Expr parseExpr();
};

#endif