#include "lexer.h"
#include <stdexcept>
#include <cctype>

Lexer::Lexer(const std::string &source)
    : m_source(source), m_pos(0), m_line(1) {}

char Lexer::peek()
{
    if (isAtEnd())
        return '\0';
    return m_source[m_pos];
}

char Lexer::advance()
{
    char c = m_source[m_pos++];
    if (c == '\n')
        m_line++;
    return c;
}

bool Lexer::isAtEnd()
{
    return m_pos >= m_source.size();
}

void Lexer::skipWhitespace()
{
    while (!isAtEnd())
    {
        char c = peek();

        if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
        {
            advance();
            continue;
        }

        if (c == '/' && m_pos + 1 < m_source.size() && m_source[m_pos + 1] == '/')
        {
            while (!isAtEnd() && peek() != '\n')
                advance();
            continue;
        }

        break;
    }
}

Token Lexer::readNumber()
{
    std::string num;
    int line = m_line;

    while (!isAtEnd() && std::isdigit(peek()))
    {
        num += advance();
    }

    return Token(TokenType::INT_LITERAL, num, line);
}

Token Lexer::readWord()
{
    std::string word;
    int line = m_line;

    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_'))
    {
        word += advance();
    }

    if (word == "let")
        return Token(TokenType::LET, word, line);
    if (word == "print")
        return Token(TokenType::PRINT, word, line);
    if (word == "input")
        return Token(TokenType::INPUT, word, line);
    if (word == "if")
        return Token(TokenType::IF, word, line);
    if (word == "else")
        return Token(TokenType::ELSE, word, line);
    if (word == "while")
        return Token(TokenType::WHILE, word, line);
    if (word == "true")
        return Token(TokenType::TRUE_KW, word, line);
    if (word == "false")
        return Token(TokenType::FALSE_KW, word, line);

    return Token(TokenType::IDENTIFIER, word, line);
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (true)
    {
        skipWhitespace();

        if (isAtEnd())
        {
            tokens.push_back(Token(TokenType::EOF_TOKEN, "", m_line));
            break;
        }

        char c = peek();
        int line = m_line;

        if (std::isdigit(c))
        {
            tokens.push_back(readNumber());
            continue;
        }

        if (std::isalpha(c) || c == '_')
        {
            tokens.push_back(readWord());
            continue;
        }

        switch (c)
        {
        case '+':
            advance();
            tokens.push_back(Token(TokenType::PLUS, "+", line));
            break;
        case '-':
            advance();
            tokens.push_back(Token(TokenType::MINUS, "-", line));
            break;
        case '*':
            advance();
            tokens.push_back(Token(TokenType::STAR, "*", line));
            break;
        case '/':
            advance();
            tokens.push_back(Token(TokenType::SLASH, "/", line));
            break;
        case '<':
            advance();
            tokens.push_back(Token(TokenType::LT, "<", line));
            break;
        case '(':
            advance();
            tokens.push_back(Token(TokenType::LPAREN, "(", line));
            break;
        case ')':
            advance();
            tokens.push_back(Token(TokenType::RPAREN, ")", line));
            break;
        case '{':
            advance();
            tokens.push_back(Token(TokenType::LBRACE, "{", line));
            break;
        case '}':
            advance();
            tokens.push_back(Token(TokenType::RBRACE, "}", line));
            break;
        case ';':
            advance();
            tokens.push_back(Token(TokenType::SEMICOLON, ";", line));
            break;

        case '=':
            advance();
            if (!isAtEnd() && peek() == '=')
            {
                advance();
                tokens.push_back(Token(TokenType::EQ_EQ, "==", line));
            }
            else
            {
                tokens.push_back(Token(TokenType::EQUALS, "=", line));
            }
            break;

        default:
            throw std::runtime_error(
                "Lexer error: unexpected character '" + std::string(1, c) +
                "' on line " + std::to_string(m_line));
        }
    }

    return tokens;
}
