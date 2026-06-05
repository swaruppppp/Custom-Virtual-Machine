#pragma once
#include "ast.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

enum class OpCode : uint8_t
{
    CONST_INT = 0x01,
    CONST_TRUE = 0x02,
    CONST_FALSE = 0x03,

    ADD = 0x10,
    SUB = 0x11,
    MUL = 0x12,
    DIV = 0x13,
    EQ = 0x14,
    LT = 0x15,

    LOAD = 0x20,
    STORE = 0x21,

    JUMP = 0x30,
    JUMP_IF_FALSE = 0x31,

    PRINT = 0x40,
    INPUT = 0x41,
    POP = 0x50,
    HALT = 0xFF
};

class Compiler
{
public:
    std::vector<uint8_t> compile(const Program &program, bool resetState = true);
    static std::string disassemble(const std::vector<uint8_t> &code);

private:
    std::vector<uint8_t> m_code;
    std::unordered_map<std::string, int> m_variables;
    int m_nextSlot = 0;

    void emit(OpCode op);
    void emitByte(uint8_t b);
    void emit16(uint16_t val);
    void emit32(int32_t val);
    void patch16(size_t where, uint16_t val);
    int getSlot(const std::string &name);
    void compileNode(const ASTNode *node);
};
