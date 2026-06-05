#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token> &tokens)
    : m_tokens(tokens), m_pos(0) {}

const Token &Parser::current()
{
    return m_tokens[m_pos];
}

Token Parser::eat()
{
    return m_tokens[m_pos++];
}

bool Parser::is(TokenType t)
{
    return current().type == t;
}

Token Parser::expect(TokenType t, const std::string &msg)
{
    if (is(t))
        return eat();
    throw std::runtime_error(
        "Line " + std::to_string(current().line) + ": " + msg +
        " (got '" + current().value + "')");
}

std::unique_ptr<Program> Parser::parse()
{
    return parseProgram();
}

std::unique_ptr<Program> Parser::parseProgram()
{
    std::vector<ASTNodePtr> stmts;
    while (!is(TokenType::EOF_TOKEN))
    {
        stmts.push_back(parseStatement());
    }
    return std::make_unique<Program>(std::move(stmts));
}

ASTNodePtr Parser::parseStatement()
{
    if (is(TokenType::LET))
    {
        eat();
        Token name = expect(TokenType::IDENTIFIER, "expected variable name after 'let'");
        expect(TokenType::EQUALS, "expected '=' in let statement");
        ASTNodePtr value = parseExpression();
        expect(TokenType::SEMICOLON, "expected ';' after let statement");
        return std::make_unique<LetStatement>(name.value, std::move(value));
    }

    if (is(TokenType::PRINT))
    {
        eat();
        ASTNodePtr expr = parseExpression();
        expect(TokenType::SEMICOLON, "expected ';' after print");
        return std::make_unique<PrintStatement>(std::move(expr));
    }

    if (is(TokenType::IF))
    {
        eat();
        expect(TokenType::LPAREN, "expected '(' after 'if'");
        ASTNodePtr cond = parseExpression();
        expect(TokenType::RPAREN, "expected ')' after condition");

        ASTNodePtr thenBlock = parseBlock();

        ASTNodePtr elseBlock = nullptr;
        if (is(TokenType::ELSE))
        {
            eat();
            elseBlock = parseBlock();
        }

        return std::make_unique<IfStatement>(
            std::move(cond), std::move(thenBlock), std::move(elseBlock));
    }

    if (is(TokenType::WHILE))
    {
        eat();
        expect(TokenType::LPAREN, "expected '(' after 'while'");
        ASTNodePtr cond = parseExpression();
        expect(TokenType::RPAREN, "expected ')' after condition");
        ASTNodePtr body = parseBlock();
        return std::make_unique<WhileStatement>(std::move(cond), std::move(body));
    }

    if (is(TokenType::LBRACE))
    {
        return parseBlock();
    }

    if (is(TokenType::IDENTIFIER) &&
        m_pos + 1 < m_tokens.size() &&
        m_tokens[m_pos + 1].type == TokenType::EQUALS)
    {

        Token name = eat();
        eat();
        ASTNodePtr value = parseExpression();
        expect(TokenType::SEMICOLON, "expected ';' after assignment");
        return std::make_unique<AssignStatement>(name.value, std::move(value));
    }

    ASTNodePtr expr = parseExpression();
    expect(TokenType::SEMICOLON, "expected ';'");
    return std::make_unique<PrintStatement>(std::move(expr));
}

ASTNodePtr Parser::parseBlock()
{
    expect(TokenType::LBRACE, "expected '{'");
    std::vector<ASTNodePtr> stmts;
    while (!is(TokenType::RBRACE) && !is(TokenType::EOF_TOKEN))
    {
        stmts.push_back(parseStatement());
    }
    expect(TokenType::RBRACE, "expected '}'");
    return std::make_unique<BlockStatement>(std::move(stmts));
}

ASTNodePtr Parser::parseExpression()
{
    return parseComparison();
}

ASTNodePtr Parser::parseComparison()
{
    ASTNodePtr left = parseTerm();
    while (is(TokenType::EQ_EQ) || is(TokenType::LT))
    {
        std::string op = eat().value;
        ASTNodePtr right = parseTerm();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parseTerm()
{
    ASTNodePtr left = parseFactor();
    while (is(TokenType::PLUS) || is(TokenType::MINUS))
    {
        std::string op = eat().value;
        ASTNodePtr right = parseFactor();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parseFactor()
{
    ASTNodePtr left = parsePrimary();
    while (is(TokenType::STAR) || is(TokenType::SLASH))
    {
        std::string op = eat().value;
        ASTNodePtr right = parsePrimary();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parsePrimary()
{
    if (is(TokenType::INT_LITERAL))
    {
        int val = std::stoi(eat().value);
        return std::make_unique<NumberLiteral>(val);
    }

    if (is(TokenType::TRUE_KW))
    {
        eat();
        return std::make_unique<BoolLiteral>(true);
    }
    if (is(TokenType::FALSE_KW))
    {
        eat();
        return std::make_unique<BoolLiteral>(false);
    }

    if (is(TokenType::INPUT))
    {
        eat();
        return std::make_unique<InputExpr>();
    }

    if (is(TokenType::IDENTIFIER))
    {
        return std::make_unique<Identifier>(eat().value);
    }

    if (is(TokenType::LPAREN))
    {
        eat();
        ASTNodePtr expr = parseExpression();
        expect(TokenType::RPAREN, "expected ')'");
        return expr;
    }

    throw std::runtime_error(
        "Line " + std::to_string(current().line) +
        ": unexpected '" + current().value + "'");
}
