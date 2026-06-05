#include "compiler.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstring>

void Compiler::emit(OpCode op) { m_code.push_back(static_cast<uint8_t>(op)); }
void Compiler::emitByte(uint8_t b) { m_code.push_back(b); }

void Compiler::emit16(uint16_t val)
{
    emitByte(val & 0xFF);
    emitByte((val >> 8) & 0xFF);
}

void Compiler::emit32(int32_t val)
{
    uint32_t u;
    std::memcpy(&u, &val, sizeof(u));
    emitByte(u & 0xFF);
    emitByte((u >> 8) & 0xFF);
    emitByte((u >> 16) & 0xFF);
    emitByte((u >> 24) & 0xFF);
}

void Compiler::patch16(size_t where, uint16_t val)
{
    m_code[where] = val & 0xFF;
    m_code[where + 1] = (val >> 8) & 0xFF;
}

int Compiler::getSlot(const std::string &name)
{
    auto it = m_variables.find(name);
    if (it != m_variables.end())
        return it->second;
    int slot = m_nextSlot++;
    m_variables[name] = slot;
    return slot;
}

std::vector<uint8_t> Compiler::compile(const Program &program, bool resetState)
{
    m_code.clear();
    if (resetState) {
        m_variables.clear();
        m_nextSlot = 0;
    }
    for (auto &stmt : program.statements)
        compileNode(stmt.get());
    emit(OpCode::HALT);
    return m_code;
}

void Compiler::compileNode(const ASTNode *node)
{
    switch (node->type)
    {

    case NodeType::NUMBER_LITERAL:
    {
        auto *n = static_cast<const NumberLiteral *>(node);
        emit(OpCode::CONST_INT);
        emit32(n->value);
        break;
    }

    case NodeType::BOOL_LITERAL:
    {
        auto *b = static_cast<const BoolLiteral *>(node);
        emit(b->value ? OpCode::CONST_TRUE : OpCode::CONST_FALSE);
        break;
    }

    case NodeType::IDENTIFIER:
    {
        auto *id = static_cast<const Identifier *>(node);
        emit(OpCode::LOAD);
        emit16(static_cast<uint16_t>(getSlot(id->name)));
        break;
    }

    case NodeType::INPUT_EXPR:
        emit(OpCode::INPUT);
        break;

    case NodeType::BINARY_EXPR:
    {
        auto *bin = static_cast<const BinaryExpr *>(node);
        compileNode(bin->left.get());
        compileNode(bin->right.get());
        if (bin->op == "+")
            emit(OpCode::ADD);
        else if (bin->op == "-")
            emit(OpCode::SUB);
        else if (bin->op == "*")
            emit(OpCode::MUL);
        else if (bin->op == "/")
            emit(OpCode::DIV);
        else if (bin->op == "==")
            emit(OpCode::EQ);
        else if (bin->op == "<")
            emit(OpCode::LT);
        else
            throw std::runtime_error("Unknown operator: " + bin->op);
        break;
    }

    case NodeType::LET_STMT:
    {
        auto *let = static_cast<const LetStatement *>(node);
        compileNode(let->initializer.get());
        emit(OpCode::STORE);
        emit16(static_cast<uint16_t>(getSlot(let->name)));
        break;
    }

    case NodeType::ASSIGN_STMT:
    {
        auto *assign = static_cast<const AssignStatement *>(node);
        compileNode(assign->value.get());
        emit(OpCode::STORE);
        emit16(static_cast<uint16_t>(getSlot(assign->name)));
        break;
    }

    case NodeType::PRINT_STMT:
    {
        auto *pr = static_cast<const PrintStatement *>(node);
        compileNode(pr->expression.get());
        emit(OpCode::PRINT);
        break;
    }

    case NodeType::BLOCK_STMT:
    {
        auto *block = static_cast<const BlockStatement *>(node);
        for (auto &s : block->statements)
            compileNode(s.get());
        break;
    }

    case NodeType::IF_STMT:
    {
        auto *ifS = static_cast<const IfStatement *>(node);
        compileNode(ifS->condition.get());
        emit(OpCode::JUMP_IF_FALSE);
        size_t falseJump = m_code.size();
        emit16(0);
        compileNode(ifS->thenBranch.get());
        if (ifS->elseBranch)
        {
            emit(OpCode::JUMP);
            size_t endJump = m_code.size();
            emit16(0);
            patch16(falseJump, static_cast<uint16_t>(m_code.size()));
            compileNode(ifS->elseBranch.get());
            patch16(endJump, static_cast<uint16_t>(m_code.size()));
        }
        else
        {
            patch16(falseJump, static_cast<uint16_t>(m_code.size()));
        }
        break;
    }

    case NodeType::WHILE_STMT:
    {
        auto *whS = static_cast<const WhileStatement *>(node);
        size_t loopStart = m_code.size();
        compileNode(whS->condition.get());
        emit(OpCode::JUMP_IF_FALSE);
        size_t exitJump = m_code.size();
        emit16(0);
        compileNode(whS->body.get());
        emit(OpCode::JUMP);
        emit16(static_cast<uint16_t>(loopStart));
        patch16(exitJump, static_cast<uint16_t>(m_code.size()));
        break;
    }

    case NodeType::PROGRAM:
    {
        auto *prog = static_cast<const Program *>(node);
        for (auto &s : prog->statements)
            compileNode(s.get());
        break;
    }

    default:
        throw std::runtime_error("Compiler: unknown node type");
    }
}

std::string Compiler::disassemble(const std::vector<uint8_t> &code)
{
    std::ostringstream out;
    size_t i = 0;

    auto r16 = [&]() -> uint16_t
    {
        uint16_t v = code[i] | (uint16_t(code[i + 1]) << 8);
        i += 2;
        return v;
    };
    auto r32 = [&]() -> int32_t
    {
        uint32_t v = code[i] | (uint32_t(code[i + 1]) << 8) | (uint32_t(code[i + 2]) << 16) | (uint32_t(code[i + 3]) << 24);
        i += 4;
        int32_t r;
        std::memcpy(&r, &v, sizeof(r));
        return r;
    };

    while (i < code.size())
    {
        out << std::setw(4) << std::setfill('0') << i << "  ";
        auto op = static_cast<OpCode>(code[i++]);
        switch (op)
        {
        case OpCode::CONST_INT:
            out << "CONST_INT     " << r32() << "\n";
            break;
        case OpCode::CONST_TRUE:
            out << "CONST_TRUE\n";
            break;
        case OpCode::CONST_FALSE:
            out << "CONST_FALSE\n";
            break;
        case OpCode::ADD:
            out << "ADD\n";
            break;
        case OpCode::SUB:
            out << "SUB\n";
            break;
        case OpCode::MUL:
            out << "MUL\n";
            break;
        case OpCode::DIV:
            out << "DIV\n";
            break;
        case OpCode::EQ:
            out << "EQ\n";
            break;
        case OpCode::LT:
            out << "LT\n";
            break;
        case OpCode::LOAD:
            out << "LOAD          slot:" << r16() << "\n";
            break;
        case OpCode::STORE:
            out << "STORE         slot:" << r16() << "\n";
            break;
        case OpCode::JUMP:
            out << "JUMP          addr:" << r16() << "\n";
            break;
        case OpCode::JUMP_IF_FALSE:
            out << "JUMP_IF_FALSE addr:" << r16() << "\n";
            break;
        case OpCode::PRINT:
            out << "PRINT\n";
            break;
        case OpCode::INPUT:
            out << "INPUT\n";
            break;
        case OpCode::POP:
            out << "POP\n";
            break;
        case OpCode::HALT:
            out << "HALT\n";
            break;
        default:
            out << "???\n";
            break;
        }
    }
    return out.str();
}
