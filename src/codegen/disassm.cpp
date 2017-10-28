#include <sstream>
#include <iomanip>
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

using std::setw;
using std::setfill;
static inline
std::string get_n_bytes(byte *p, size_t n, size_t min_width=10*3)
{
    std::stringstream ss;
    size_t column = 0;
    for (size_t i = 0; i < n; ++i)
    {
        ss << setfill('0') << setw(2) << std::hex << (int)p[i] << " ";
        column += 3;
    }
    for (; column < min_width; ++column)
    {
        ss << ' ';
    }
    return ss.str();
}

std::string Disassembler::dis(std::vector<byte> code)
{
    std::stringstream ss;

    auto p = code.data();
    auto begin = p;
    auto end = &code[code.size()];
    while (p != end)
    {
        auto ins = static_cast<Instruction>(fetch8(p));
        auto ins_str = to_string(ins);
        ss << setfill('0') << setw(8) << std::hex << p - begin << "  ";
        ss << setfill(' ');
        switch (ins)
        {
            case Instruction::Literal_8:
            {
                ss << get_n_bytes(p, 2);
                ++p;
                auto n = fetch8(p);
                ss << ins_str << " <" << n << ">";
                p += sizeof(n);
            } break;
            case Instruction::Literal_16:
            case Instruction::Load_Local:
            case Instruction::Store_Local:
            case Instruction::Alloc_Locals:
            case Instruction::Drop_N:
            {
                ss << get_n_bytes(p, 3);
                ++p;
                auto n = fetch16(p);
                ss << ins_str << " <" << n << ">";
                p += sizeof(n);
            } break;
            case Instruction::Load_Global:
            case Instruction::Store_Global:
            case Instruction::Literal_32:
            case Instruction::Call:
            case Instruction::Call_Primitive:
            case Instruction::Array_New:
            case Instruction::Load_String_Constant:
            {
                ss << get_n_bytes(p, 5);
                ++p;
                auto n = fetch32(p);
                ss << ins_str <<" <" << n << ">";
                p += sizeof(n);
            } break;
            case Instruction::Branch:
            case Instruction::Branch_If_False_Or_Pop:
            case Instruction::Pop_Branch_If_False:
            case Instruction::Branch_If_True_Or_Pop:
            case Instruction::Pop_Branch_If_True:
            {
                ss << get_n_bytes(p, 5);
                ++p;
                auto n = fetch32(p);
                ss << ins_str << "(" << n << ") -> ";
                auto instr = p-1;
                auto dest = instr + n;
                ss << dest - begin;
                p += sizeof(n);
            } break;
            case Instruction::Literal_value:
            {
                ss << get_n_bytes(p, 9);
                ++p;
                auto n = fetch_value(p);
                ss << ins_str;
                if (n.is_object())      {ss << " object:<" << n.as_object();}
                else if (n.is_double()) {ss << " double:<" << n.as_double();}
                else if (n.is_fixnum()) {ss << " fixnum:<" << n.as_fixnum();}
                else                    {ss << " ??:<" << n.bits();}
                ss << ">";
                p += sizeof(n);
            } break;
            default:
            {
                ss << get_n_bytes(p, 1);
                ss << ins_str;
                ++p;
            } break;
        }
        ss << std::endl;
    }
    return ss.str();
}
