#include <sstream>
#include "disassm.hpp"

static inline byte fetch1(byte *p)
{
    return *p;
}

static inline int16_t fetch2(byte *p)
{
    return *reinterpret_cast<int16_t*>(p);
}

static inline int32_t fetch4(byte *p)
{
    return *reinterpret_cast<int32_t*>(p);
}

static inline intptr_t fetch_int(byte *p)
{
    return *reinterpret_cast<intptr_t*>(p);
}

std::string Disassembler::dis(std::vector<byte> code)
{
    std::stringstream ss;

    auto p = code.data();
    auto end = &code[code.size()];
    while (p != end)
    {
        auto ins = static_cast<Instruction>(fetch1(p));
        auto ins_str = instruction_to_string(ins);
        ss << ins_str;
        if (ins == Instruction::Literal_1)
        {
            ++p;
            auto n = fetch1(p);
            ss << "<" << n << ">";
            ++p;
        }
        else if (ins == Instruction::Literal_2)
        {
            ++p;
            auto n = fetch2(p);
            ss << "<" << n << ">";
            p += 2;
        }
        else if (ins == Instruction::Literal_4)
        {
            ++p;
            auto n = fetch4(p);
            ss << "<" << n << ">";
            p += 4;
        }
        else if (ins == Instruction::Literal_int)
        {
            ++p;
            auto n = fetch_int(p);
            ss << "<" << n << ">";
            p += sizeof(n);
        }
        else if (ins == Instruction::Branch)
        {
            ++p;
            auto n = fetch_int(p);
            ss << "<" << n << ">";
            p += sizeof(n);
        }
        else if (ins == Instruction::Branch_If_Zero)
        {
            ++p;
            auto n = fetch_int(p);
            ss << "<" << n << ">";
            p += sizeof(n);
        }
        else
        {
            ++p;
        }
        ss << std::endl;
    }
    return ss.str();
}
