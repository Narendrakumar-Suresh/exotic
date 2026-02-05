#include "parser.h"
#include <iostream>
#include <cstdlib>

using namespace std;

Parser::Parser(Lexer& lexer)
    : lexer(lexer) {
    advance();
}

void Parser::advance() {
    current = lexer.nextToken();
}

void Parser::expect(TokenKind kind) {
    if (current.kind != kind) {
        cerr << "Parse error\n";
        exit(1);
    }
    advance();
}

LetStmt Parser::parseLet() {
    expect(TokenKind::KeywordLet);

    string name = current.text;
    expect(TokenKind::Identifier);

    expect(TokenKind::Equal);

    Expr expr = parseExpr(); 

    expect(TokenKind::Semicolon);

    symbols.set(name, expr.value);

    return { name, expr };
}


Expr Parser::parseExpr(){
    int value=0;

    if (current.kind == TokenKind::Number) {
        value = stoi(current.text);
        advance();
    }else if(current.kind == TokenKind::Identifier){
        value = symbols.get(current.text);
        advance();
    } 
    
    while (current.kind == TokenKind::Plus) {
        advance(); 

        int rhs = 0;
        if (current.kind == TokenKind::Number) {
            rhs = stoi(current.text);
            advance();
        } else if (current.kind == TokenKind::Identifier) {
            rhs = symbols.get(current.text);
            advance();
        }

        value += rhs;
    }

    return {value};
}

vector<LetStmt> Parser::parseProgram() {
    vector<LetStmt> program;

    while (current.kind != TokenKind::EndOfFile) {
        program.push_back(parseLet());
    }

    return program;
}

Type Parser::parseType() {
    string t = current.text;
    expect(TokenKind::Identifier);

    if (t == "i32") return I32();
    if (t == "i64") return I64();
    if (t == "f64") return F64();
    if (t == "string") return String();

    if (t == "list") {
        expect(TokenKind::LBracket);
        Type elem = parseType();
        expect(TokenKind::RBracket);
        return List(new Type(elem));
    }

    cerr << "Unknown type\n";
    exit(1);
}
