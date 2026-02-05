#include "lexer.h"
#include <cctype>
using namespace std;

Lexer::Lexer(const string& source):
    source(source),
    pos(0),
    line(1),
    column(1){}

char Lexer::peek(){
    if (pos >= source.size()){
        return '\0';
    }
    return source[pos];
}

char Lexer::peekNext(){
    if (pos + 1 >= source.size()){
        return '\0';
    }
    return source[pos + 1];
}

char Lexer::advance(){
    if (pos >= source.size()){
        return '\0';
    }
    char c = source[pos++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        advance();
    }
}

Token Lexer::nextToken(){
    skipWhitespace();

    int tokenLine = line;
    int tokenColumn = column;
    char c = peek();

    if (c == '\0') {
        return { TokenKind::EndOfFile, "", tokenLine, tokenColumn };
    }

    if (isdigit(c)){
        string value;
        while (isdigit(peek()) || peek() == '.') {
            value += advance();
        }
        return { TokenKind::Number, value, tokenLine, tokenColumn };
    }

    if (c == '"') {
        advance();
        string value;
        while (peek() != '"' && peek() != '\0') {
            if (peek() == '\\') {
                advance();
                char escaped = peek();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    default: value += escaped; break;
                }
                advance();
            } else {
                value += advance();
            }
        }
        if (peek() == '"') {
            advance();
        }
        return { TokenKind::String, value, tokenLine, tokenColumn };
    }

    if (std::isalpha(c) || c == '_') {
        std::string value;
        while (std::isalnum(peek()) || peek() == '_') {
            value += advance();
        }

        if (value == "let") {
            return { TokenKind::KeywordLet, value, tokenLine, tokenColumn };
        }
        if (value == "print") {
            return { TokenKind::KeywordPrint, value, tokenLine, tokenColumn };
        }
        if (value == "i8") {
            return { TokenKind::KeywordI8, value, tokenLine, tokenColumn };
        }
        if (value == "i16") {
            return { TokenKind::KeywordI16, value, tokenLine, tokenColumn };
        }
        if (value == "i32") {
            return { TokenKind::KeywordI32, value, tokenLine, tokenColumn };
        }
        if (value == "i64") {
            return { TokenKind::KeywordI64, value, tokenLine, tokenColumn };
        }
        if (value == "f32") {
            return { TokenKind::KeywordF32, value, tokenLine, tokenColumn };
        }
        if (value == "f64") {
            return { TokenKind::KeywordF64, value, tokenLine, tokenColumn };
        }
        if (value == "string") {
            return { TokenKind::KeywordString, value, tokenLine, tokenColumn };
        }
        if (value == "list") {
            return { TokenKind::KeywordList, value, tokenLine, tokenColumn };
        }

        return { TokenKind::Identifier, value, tokenLine, tokenColumn };
    }

    advance();
    switch (c) {
    case '+': return { TokenKind::Plus, "+", tokenLine, tokenColumn };
    case '-': return { TokenKind::Minus, "-", tokenLine, tokenColumn };
    case '*': return { TokenKind::Star, "*", tokenLine, tokenColumn };
    case '/':
        if (peek() == '/') {
            advance();
            return { TokenKind::DoubleSlash, "//", tokenLine, tokenColumn };
        }
        return { TokenKind::Slash, "/", tokenLine, tokenColumn };
    case '%': return { TokenKind::Percent, "%", tokenLine, tokenColumn };
    case '=':
        if (peek() == '=') {
            advance();
            return { TokenKind::EqualEqual, "==", tokenLine, tokenColumn };
        }
        return { TokenKind::Equal, "=", tokenLine, tokenColumn };
    case '!':
        if (peek() == '=') {
            advance();
            return { TokenKind::NotEqual, "!=", tokenLine, tokenColumn };
        }
        return { TokenKind::Bang, "!", tokenLine, tokenColumn };
    case '<':
        if (peek() == '=') {
            advance();
            return { TokenKind::LessEqual, "<=", tokenLine, tokenColumn };
        }
        return { TokenKind::Less, "<", tokenLine, tokenColumn };
    case '>':
        if (peek() == '=') {
            advance();
            return { TokenKind::GreaterEqual, ">=", tokenLine, tokenColumn };
        }
        return { TokenKind::Greater, ">", tokenLine, tokenColumn };
    case '&':
        if (peek() == '&') {
            advance();
            return { TokenKind::AmpersandAmpersand, "&&", tokenLine, tokenColumn };
        }
        return { TokenKind::Ampersand, "&", tokenLine, tokenColumn };
    case '|':
        if (peek() == '|') {
            advance();
            return { TokenKind::PipePipe, "||", tokenLine, tokenColumn };
        }
        return { TokenKind::Pipe, "|", tokenLine, tokenColumn };
    case ':': return { TokenKind::Colon, ":", tokenLine, tokenColumn };
    case ';': return { TokenKind::Semicolon, ";", tokenLine, tokenColumn };
    case ',': return { TokenKind::Comma, ",", tokenLine, tokenColumn };
    case '(': return { TokenKind::LParen, "(", tokenLine, tokenColumn };
    case ')': return { TokenKind::RParen, ")", tokenLine, tokenColumn };
    case '[': return { TokenKind::LBracket, "[", tokenLine, tokenColumn };
    case ']': return { TokenKind::RBracket, "]", tokenLine, tokenColumn };
    case '.': return { TokenKind::Dot, ".", tokenLine, tokenColumn };
    default:
        return { TokenKind::Unknown, std::string(1, c), tokenLine, tokenColumn };
    }
}