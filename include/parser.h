#pragma once
#include "token.h"
#include "ast.h"
#include <vector>

class Parser
{
public:
    Parser(const std::vector<Token> &tokens);
    std::unique_ptr<Program> parse();

private:
    std::vector<Token> m_tokens;
    size_t m_pos;

    const Token &current();
    Token eat();
    bool is(TokenType t);
    Token expect(TokenType t, const std::string &msg);

    std::unique_ptr<Program> parseProgram();
    ASTNodePtr parseStatement();
    ASTNodePtr parseBlock();
    ASTNodePtr parseExpression();
    ASTNodePtr parseComparison();
    ASTNodePtr parseTerm();
    ASTNodePtr parseFactor();
    ASTNodePtr parsePrimary();
};
