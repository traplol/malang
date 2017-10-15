#include "instruction.hpp"


std::string to_string(Instruction instruction)
{
    switch (instruction)
    {
#define ITEM(X) case Instruction::X: return #X ;
        #include "instruction.def"
        case Instruction::INSTRUCTION_ENUM_SIZE : return "INSTRUCTION_ENUM_SIZE";
    }
    return "Out of range";
}
