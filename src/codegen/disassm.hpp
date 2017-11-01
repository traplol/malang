#ifndef MALANG_CODEGEN_DISASSM_H
#define MALANG_CODEGEN_DISASSM_H

#include <vector>
#include <string>
#include "../vm/instruction.hpp"

struct Disassembler
{
    static std::string dis(std::vector<byte> code);
    static byte *dis1(byte *p, int offset, std::string &out);
};

#endif /* MALANG_CODEGEN_DISASSM_H */
