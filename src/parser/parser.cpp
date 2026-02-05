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
        cerr << "Parse error at line " << current.line << ", column " << current.column 
             << ": expected different token\n";
        exit(1);
    }
    advance();
}

unique_ptr<LetStmt> Parser::parseLet() {
    expect(TokenKind::KeywordLet);

    string name = current.text;
    expect(TokenKind::Identifier);

    auto stmt = make_unique<LetStmt>();
    stmt->name = name;

    if (current.kind == TokenKind::Colon) {
        advance(); // Consume the colon
        stmt->declared_type = parseType(); // Parse the type
    }
    
    expect(TokenKind::Equal);

    auto expr = parseExpr();
    stmt->expr = std::move(expr);
    
    return stmt;
}

unique_ptr<Expr> Parser::parseExpr() {
    return parseLogicalOr();
}

unique_ptr<Expr> Parser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    
    while (current.kind == TokenKind::PipePipe) {
        string op = current.text;
        advance();
        auto right = parseLogicalAnd();
        
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::Binary;
        node->op = op;
        node->left = std::move(left);
        node->right = std::move(right);
        left = std::move(node);
    }
    
    return left;
}

unique_ptr<Expr> Parser::parseLogicalAnd() {
    auto left = parseEquality();
    
    while (current.kind == TokenKind::AmpersandAmpersand) {
        string op = current.text;
        advance();
        auto right = parseEquality();
        
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::Binary;
        node->op = op;
        node->left = std::move(left);
        node->right = std::move(right);
        left = std::move(node);
    }
    
    return left;
}

unique_ptr<Expr> Parser::parseEquality() {
    auto left = parseComparison();
    
    while (current.kind == TokenKind::EqualEqual || current.kind == TokenKind::NotEqual) {
        string op = current.text;
        advance();
        auto right = parseComparison();
        
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::Binary;
        node->op = op;
        node->left = std::move(left);
        node->right = std::move(right);
        left = std::move(node);
    }
    
    return left;
}

unique_ptr<Expr> Parser::parseComparison() {
    auto left = parseTerm();
    
    while (current.kind == TokenKind::Less || current.kind == TokenKind::LessEqual ||
           current.kind == TokenKind::Greater || current.kind == TokenKind::GreaterEqual) {
        string op = current.text;
        advance();
        auto right = parseTerm();
        
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::Binary;
        node->op = op;
        node->left = std::move(left);
        node->right = std::move(right);
        left = std::move(node);
    }
    
    return left;
}

unique_ptr<Expr> Parser::parseTerm() {
    auto left = parseFactor();
    
    while (current.kind == TokenKind::Plus || current.kind == TokenKind::Minus) {
        string op = current.text;
        advance();
        auto right = parseFactor();
        
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::Binary;
        node->op = op;
        node->left = std::move(left);
        node->right = std::move(right);
        left = std::move(node);
    }
    
    return left;
}

unique_ptr<Expr> Parser::parseFactor() {
    auto left = parseUnary();
    
    while (current.kind == TokenKind::Star || current.kind == TokenKind::Slash || 
           current.kind == TokenKind::DoubleSlash || current.kind == TokenKind::Percent) {
        string op = current.text;
        advance();
        auto right = parseUnary();
        
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::Binary;
        node->op = op;
        node->left = std::move(left);
        node->right = std::move(right);
        left = std::move(node);
    }
    
    return left;
}

unique_ptr<Expr> Parser::parseUnary() {
    if (current.kind == TokenKind::Minus || current.kind == TokenKind::Bang) {
        string op = current.text;
        advance();
        auto operand = parseUnary();
        
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::Unary;
        node->op = op;
        node->left = std::move(operand);
        return node;
    }
    
    return parsePostfix();
}

unique_ptr<Expr> Parser::parsePostfix() {
    auto expr = parsePrimary();
    
    while (true) {
        if (current.kind == TokenKind::LBracket) {
            advance();
            
            if (current.kind == TokenKind::Colon) {
                advance();
                auto end = parseExpr();
                expect(TokenKind::RBracket);
                
                auto node = make_unique<Expr>(current.line, current.column);
                node->kind = ExprKind::StringSlice;
                node->left = std::move(expr);
                node->start = nullptr;
                node->end = std::move(end);
                expr = std::move(node);
                
            } else {
                auto start = parseExpr();
                
                if (current.kind == TokenKind::Colon) {
                    advance();
                    
                    unique_ptr<Expr> end = nullptr;
                    if (current.kind != TokenKind::RBracket) {
                        end = parseExpr();
                    }
                    expect(TokenKind::RBracket);
                    
                    auto node = make_unique<Expr>(current.line, current.column);
                    node->kind = ExprKind::StringSlice;
                    node->left = std::move(expr);
                    node->start = std::move(start);
                    node->end = std::move(end);
                    expr = std::move(node);
                    
                } else {
                    expect(TokenKind::RBracket);
                    
                    auto node = make_unique<Expr>(current.line, current.column);
                    node->kind = ExprKind::StringSlice;
                    node->left = std::move(expr);
                    node->start = std::move(start);
                    node->end = make_unique<Expr>(current.line, current.column); // Also update this internal expr
                    node->end->kind = ExprKind::NumberLiteral;
                    node->end->int_value = -1;
                    expr = std::move(node);
                }
            }
            
        } else if (current.kind == TokenKind::Dot) {
            advance();
            string method = current.text;
            expect(TokenKind::Identifier);
            
            expect(TokenKind::LParen);
            vector<unique_ptr<Expr>> args;
            
            if (current.kind != TokenKind::RParen) {
                args.push_back(parseExpr());
                while (current.kind == TokenKind::Plus) {
                    advance();
                    args.push_back(parseExpr());
                }
            }
            expect(TokenKind::RParen);
            
            auto node = make_unique<Expr>(current.line, current.column);
            node->kind = ExprKind::MethodCall;
            node->left = std::move(expr);
            node->method_name = method;
            node->args = std::move(args);
            expr = std::move(node);
            
        } else {
            break;
        }
    }
    
    return expr;
}

unique_ptr<Expr> Parser::parsePrimary() {
    if (current.kind == TokenKind::Number) {
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::NumberLiteral;
        string text = current.text;
        
        if (text.find('.') != string::npos) {
            node->double_value = stod(text);
            node->type = f64Type();
        } else {
            node->int_value = stoi(text);
            node->type = i32Type();
        }
        advance();
        return node;
        
    } else if (current.kind == TokenKind::String) {
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::StringLiteral;
        node->string_value = current.text;
        node->type = stringType();
        advance();
        return node;
        
    } else if (current.kind == TokenKind::Identifier) {
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::Identifier;
        node->string_value = current.text;
        advance();
        return node;
        
    } else if (current.kind == TokenKind::LParen) {
        advance();
        auto expr = parseExpr();
        expect(TokenKind::RParen);
        return expr;
    } else if (current.kind == TokenKind::LBracket) {
        advance(); // Consume the LBracket
        auto node = make_unique<Expr>(current.line, current.column);
        node->kind = ExprKind::ListLiteral;
        // The type for a ListLiteral will be determined during semantic analysis
        // For now, we can set a placeholder or leave it default constructed

        if (current.kind != TokenKind::RBracket) { // Check if the list is not empty
            node->elements.push_back(parseExpr());
            while (current.kind == TokenKind::Comma) {
                advance(); // Consume the comma
                node->elements.push_back(parseExpr());
            }
        }
        expect(TokenKind::RBracket); // Expect closing RBracket
        return node;
    }
    
    cerr << "Parse error at line " << current.line << ": unexpected token\n";
    exit(1);
}

unique_ptr<PrintStmt> Parser::parsePrint() {
    expect(TokenKind::KeywordPrint);

    auto expr = parseExpr();

    auto stmt = make_unique<PrintStmt>();
    stmt->expr = std::move(expr);
    
    return stmt;
}

unique_ptr<Stmt> Parser::parseStatement() {
    if (current.kind == TokenKind::KeywordLet) {
        return parseLet();
    } else if (current.kind == TokenKind::KeywordPrint) {
        return parsePrint();
    }
    
    cerr << "Parse error at line " << current.line << ": unexpected token in statement\n";
    exit(1);
}

vector<unique_ptr<Stmt>> Parser::parseProgram() {
    vector<unique_ptr<Stmt>> program;

    while (current.kind != TokenKind::EndOfFile) {
        program.push_back(parseStatement());
    }

    return program;
}

Type Parser::parseType() {
    if (current.kind == TokenKind::KeywordI8) {
        advance();
        return i8Type();
    }
    if (current.kind == TokenKind::KeywordI16) {
        advance();
        return i16Type();
    }
    if (current.kind == TokenKind::KeywordI32) { // Assuming KeywordI32 also exists or needs to be added
        advance();
        return i32Type();
    }
    if (current.kind == TokenKind::KeywordI64) {
        advance();
        return i64Type();
    }
    if (current.kind == TokenKind::KeywordF32) {
        advance();
        return f32Type();
    }
    if (current.kind == TokenKind::KeywordF64) {
        advance();
        return f64Type();
    }
    if (current.kind == TokenKind::KeywordString) {
        advance();
        return stringType();
    }
    if (current.kind == TokenKind::KeywordList) {
        advance(); // Consume "list" keyword
        expect(TokenKind::LBracket);
        Type elem = parseType();
        expect(TokenKind::RBracket);
        return listType(new Type(elem));
    }
    
    cerr << "Parse error at line " << current.line << ": unexpected token when parsing type\n";
    exit(1);
}