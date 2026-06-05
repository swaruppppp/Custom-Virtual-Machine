#include "compiler.h"
#include "vm.h"
#include <iostream>
#include <stdexcept>
#include <cstring>

void VM::push(int32_t val) { m_stack.push_back(val); }

int32_t VM::pop()
{
    if (m_stack.empty())
        throw std::runtime_error("VM error: stack underflow");
    int32_t val = m_stack.back();
    m_stack.pop_back();
    return val;
}

uint16_t VM::read16(const std::vector<uint8_t> &code)
{
    uint16_t val = code[m_ip] | (uint16_t(code[m_ip + 1]) << 8);
    m_ip += 2;
    return val;
}

int32_t VM::read32(const std::vector<uint8_t> &code)
{
    uint32_t val = code[m_ip] | (uint32_t(code[m_ip + 1]) << 8) | (uint32_t(code[m_ip + 2]) << 16) | (uint32_t(code[m_ip + 3]) << 24);
    m_ip += 4;
    int32_t r;
    std::memcpy(&r, &val, sizeof(r));
    return r;
}

void VM::run(const std::vector<uint8_t> &bytecode, bool resetGlobals)
{
    m_ip = 0;
    m_stack.clear();
    if (resetGlobals) {
        m_globals.assign(256, 0);
    } else {
        if (m_globals.size() < 256) m_globals.resize(256, 0);
    }

    while (m_ip < bytecode.size())
    {
        auto op = static_cast<OpCode>(bytecode[m_ip++]);
        switch (op)
        {
        case OpCode::CONST_INT:
            push(read32(bytecode));
            break;
        case OpCode::CONST_TRUE:
            push(1);
            break;
        case OpCode::CONST_FALSE:
            push(0);
            break;
        case OpCode::ADD:
        {
            auto b = pop(), a = pop();
            push(a + b);
            break;
        }
        case OpCode::SUB:
        {
            auto b = pop(), a = pop();
            push(a - b);
            break;
        }
        case OpCode::MUL:
        {
            auto b = pop(), a = pop();
            push(a * b);
            break;
        }
        case OpCode::DIV:
        {
            auto b = pop(), a = pop();
            if (b == 0)
                throw std::runtime_error("VM error: division by zero");
            push(a / b);
            break;
        }
        case OpCode::EQ:
        {
            auto b = pop(), a = pop();
            push(a == b ? 1 : 0);
            break;
        }
        case OpCode::LT:
        {
            auto b = pop(), a = pop();
            push(a < b ? 1 : 0);
            break;
        }
        case OpCode::LOAD:
            push(m_globals[read16(bytecode)]);
            break;
        case OpCode::STORE:
            m_globals[read16(bytecode)] = pop();
            break;
        case OpCode::JUMP:
            m_ip = read16(bytecode);
            break;
        case OpCode::JUMP_IF_FALSE:
        {
            auto t = read16(bytecode);
            if (pop() == 0)
                m_ip = t;
            break;
        }
        case OpCode::PRINT:
            std::cout << pop() << std::endl;
            break;
        case OpCode::INPUT:
        {
            int32_t val;
            std::cout << "? ";
            std::cin >> val;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                throw std::runtime_error("VM error: invalid input");
            }
            push(val);
            break;
        }
        case OpCode::POP:
            pop();
            break;
        case OpCode::HALT:
            return;
        default:
            throw std::runtime_error("VM error: unknown instruction");
        }
    }
}
