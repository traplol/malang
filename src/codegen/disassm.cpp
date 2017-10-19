#include <sstream>
#include "../vm/vm.hpp"
#include "disassm.hpp"

static inline byte fetch8(byte *p)
{
    return *p;
}

static inline int16_t fetch16(byte *p)
{
    return *reinterpret_cast<int16_t*>(p);
}

static inline int32_t fetch32(byte *p)
{
    return *reinterpret_cast<int32_t*>(p);
}

static inline Malang_Value fetch_value(byte *p)
{
    return *reinterpret_cast<Malang_Value*>(p);
}

std::string Disassembler::dis(std::vector<byte> code)
{
    std::stringstream ss;

    auto p = code.data();
    auto end = &code[code.size()];
    while (p != end)
    {
        auto ins = static_cast<Instruction>(fetch8(p));
        auto ins_str = to_string(ins);
        ss << ins_str;
        if (ins == Instruction::Literal_8)
        {
            ++p;
            auto n = fetch8(p);
            ss << " <" << n << ">";
            p += sizeof(n);
        }
        else if (ins == Instruction::Literal_16)
        {
            ++p;
            auto n = fetch16(p);
            ss << " <" << n << ">";
            p += sizeof(n);
        }
        else if (ins == Instruction::Literal_32)
        {
            ++p;
            auto n = fetch32(p);
            ss << " <" << n << ">";
            p += sizeof(n);
        }
        else if (ins == Instruction::Literal_value)
        {
            ++p;
            auto n = fetch_value(p);
            if (n.is_object())     {ss << " pointer:<" << n.as_object();}
            else if (n.is_double()) {ss << " double:<" << n.as_double();}
            else if (n.is_fixnum()) {ss << " fixnum:<" << n.as_fixnum();}
            ss << ">";
            p += sizeof(n);
        }
        else if (ins == Instruction::Branch)
        {
            ++p;
            auto n = fetch32(p);
            ss << "<" << n << ">";
            p += sizeof(n);
        }
        else if (ins == Instruction::Branch_If_Zero)
        {
            ++p;
            auto n = fetch32(p);
            ss << "<" << n << ">";
            p += sizeof(n);
        }
        else if (ins == Instruction::Call_Primitive)
        {
            ++p;
            auto n = fetch32(p);
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
