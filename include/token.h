#pragma once
#include <string>

enum class TokenType
{
    INT_LITERAL,
    IDENTIFIER,

    LET,
    PRINT,
    INPUT,
    IF,
    ELSE,
    WHILE,
    TRUE_KW,
    FALSE_KW,

    PLUS,
    MINUS,
    STAR,
    SLASH,

    EQ_EQ,
    LT,

    EQUALS,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,

    EOF_TOKEN
};

struct Token
{
    TokenType type;
    std::string value;
    int line;

    Token(TokenType t, std::string v, int ln)
        : type(t), value(std::move(v)), line(ln) {}
};
