#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "ast.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(Lexer& lexer);
    std::vector<std::unique_ptr<Stmt>> parseProgram();

private:
    Lexer& lexer;
    Token current;
    Type parseType();

    void advance();
    void expect(TokenKind kind);
    
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<LetStmt> parseLet();
    std::unique_ptr<PrintStmt> parsePrint();

    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<Expr> parseLogicalOr();
    std::unique_ptr<Expr> parseLogicalAnd();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePostfix();
    std::unique_ptr<Expr> parsePrimary();
};

#endif