#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

class VM
{
public:
    void run(const std::vector<uint8_t> &bytecode, bool resetGlobals = true);

private:
    std::vector<int32_t> m_stack;
    std::vector<int32_t> m_globals;
    size_t m_ip = 0;

    void push(int32_t val);
    int32_t pop();
    uint16_t read16(const std::vector<uint8_t> &code);
    int32_t read32(const std::vector<uint8_t> &code);
};
