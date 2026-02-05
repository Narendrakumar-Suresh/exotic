#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenKind {
    Identifier,
    Number,
    String,
    KeywordLet,
    KeywordPrint,
    KeywordI8,
    KeywordI16,
    KeywordI32,
    KeywordI64,
    KeywordF32,
    KeywordF64,
    KeywordString,
    KeywordList,
    Plus,
    Minus,
    Star,
    Slash,
    DoubleSlash,
    Percent,
    Equal,
    EqualEqual,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Ampersand,
    Pipe,
    Bang,
    AmpersandAmpersand,
    PipePipe,
    Colon,
    Semicolon,
    Comma,
    LParen,
    RParen,
    LBracket,
    RBracket,
    Dot,
    EndOfFile,
    Unknown
};

struct Token {
    TokenKind kind;
    std::string text;
    int line;
    int column;
};

#endif