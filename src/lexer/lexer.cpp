#include "lexer.h"
#include <cctype>
using namespace std;

Lexer::Lexer(const string& source):
    source(source),
    pos(0){}

char Lexer::peek(){
    if (pos>=source.size()){
        return '\0';
    }
    return source[pos];
}

char Lexer::advance(){
    if (pos>=source.size()){
        return '\0';
    }
    return source[pos++];
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        advance();
    }
}

Token Lexer::nextToken(){
    skipWhitespace();

    char c = peek();

    if (c == '\0') {
        return { TokenKind::EndOfFile, "" };
    }

    if (isdigit(c)){
        string value;
        while (isdigit(peek())) {
            value += advance();
        }
        return { TokenKind::Number, value };
    }

    if (std::isalpha(c)) {
            std::string value;
            while (std::isalnum(peek())) {
                value += advance();
            }

            if (value == "let") {
                return { TokenKind::KeywordLet, value };
            }

            return { TokenKind::Identifier, value };
        }

    advance();
    switch (c) {
    case '+': return { TokenKind::Plus, "+" };
    case '-': return { TokenKind::Minus, "-" };
    case '*': return { TokenKind::Star, "*" };
    case '/': return { TokenKind::Slash, "/" };
    case '=': return { TokenKind::Equal, "=" };
    case ';': return { TokenKind::Semicolon, ";" };
    case '(': return { TokenKind::LParen, "(" };
    case ')': return { TokenKind::RParen, ")" };
    default:
        return { TokenKind::Unknown, std::string(1, c) };
    }
}
