#ifndef LEXER_H
#define LEXER_H

#include <string>
#include "token.h"

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();

private:
    std::string source;
    size_t pos;
    int line;
    int column;

    char peek();
    char peekNext();
    char advance();
    void skipWhitespace();
};

#endif