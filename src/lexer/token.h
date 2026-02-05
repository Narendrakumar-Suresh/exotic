#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenKind {
    Identifier,
    Number,
    KeywordLet,
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    Semicolon,
    LParen,
    RParen,
    EndOfFile,
    Unknown
};

struct Token {
    TokenKind kind;
    std::string text;
};

#endif
