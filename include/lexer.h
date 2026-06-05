#pragma once
#include "token.h"
#include <vector>
#include <string>

class Lexer
{
public:
    Lexer(const std::string &source);
    std::vector<Token> tokenize();

private:
    std::string m_source;
    size_t m_pos;
    int m_line;

    char peek();
    char advance();
    bool isAtEnd();
    void skipWhitespace();
    Token readNumber();
    Token readWord();
};
