#ifndef MALANG_VM_INSTRUCTION_HPP
#define MALANG_VM_INSTRUCTION_HPP

#include <string>

using byte = unsigned char;

enum class Instruction : byte
{
#define ITEM(X, Y) X = Y,
    #include "instruction.def"
    INSTRUCTION_ENUM_SIZE
};

std::string to_string(Instruction instruction);

#endif /* MALANG_VM_INSTRUCTION_HPP */ 
