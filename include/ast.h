#pragma once
#include <memory>
#include <string>
#include <vector>

enum class NodeType
{
    NUMBER_LITERAL,
    BOOL_LITERAL,
    IDENTIFIER,
    INPUT_EXPR,
    BINARY_EXPR,
    LET_STMT,
    ASSIGN_STMT,
    PRINT_STMT,
    BLOCK_STMT,
    IF_STMT,
    WHILE_STMT,
    PROGRAM
};

struct ASTNode
{
    NodeType type;
    ASTNode(NodeType t) : type(t) {}
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

struct NumberLiteral : ASTNode
{
    int value;
    NumberLiteral(int v) : ASTNode(NodeType::NUMBER_LITERAL), value(v) {}
};

struct BoolLiteral : ASTNode
{
    bool value;
    BoolLiteral(bool v) : ASTNode(NodeType::BOOL_LITERAL), value(v) {}
};

struct Identifier : ASTNode
{
    std::string name;
    Identifier(std::string n) : ASTNode(NodeType::IDENTIFIER), name(std::move(n)) {}
};

struct InputExpr : ASTNode
{
    InputExpr() : ASTNode(NodeType::INPUT_EXPR) {}
};

struct BinaryExpr : ASTNode
{
    std::string op;
    ASTNodePtr left;
    ASTNodePtr right;

    BinaryExpr(std::string op, ASTNodePtr l, ASTNodePtr r)
        : ASTNode(NodeType::BINARY_EXPR),
          op(std::move(op)), left(std::move(l)), right(std::move(r)) {}
};

struct LetStatement : ASTNode
{
    std::string name;
    ASTNodePtr initializer;

    LetStatement(std::string n, ASTNodePtr init)
        : ASTNode(NodeType::LET_STMT),
          name(std::move(n)), initializer(std::move(init)) {}
};

struct AssignStatement : ASTNode
{
    std::string name;
    ASTNodePtr value;

    AssignStatement(std::string n, ASTNodePtr val)
        : ASTNode(NodeType::ASSIGN_STMT),
          name(std::move(n)), value(std::move(val)) {}
};

struct PrintStatement : ASTNode
{
    ASTNodePtr expression;

    PrintStatement(ASTNodePtr expr)
        : ASTNode(NodeType::PRINT_STMT), expression(std::move(expr)) {}
};

struct BlockStatement : ASTNode
{
    std::vector<ASTNodePtr> statements;

    BlockStatement(std::vector<ASTNodePtr> stmts)
        : ASTNode(NodeType::BLOCK_STMT), statements(std::move(stmts)) {}
};

struct IfStatement : ASTNode
{
    ASTNodePtr condition;
    ASTNodePtr thenBranch;
    ASTNodePtr elseBranch;

    IfStatement(ASTNodePtr cond, ASTNodePtr thenB, ASTNodePtr elseB)
        : ASTNode(NodeType::IF_STMT),
          condition(std::move(cond)),
          thenBranch(std::move(thenB)),
          elseBranch(std::move(elseB)) {}
};

struct WhileStatement : ASTNode
{
    ASTNodePtr condition;
    ASTNodePtr body;

    WhileStatement(ASTNodePtr cond, ASTNodePtr b)
        : ASTNode(NodeType::WHILE_STMT),
          condition(std::move(cond)), body(std::move(b)) {}
};

struct Program : ASTNode
{
    std::vector<ASTNodePtr> statements;

    Program(std::vector<ASTNodePtr> stmts)
        : ASTNode(NodeType::PROGRAM), statements(std::move(stmts)) {}
};
