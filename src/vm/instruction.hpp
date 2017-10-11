#ifndef MALANG_VM_INSTRUCTION_HPP
#define MALANG_VM_INSTRUCTION_HPP

#include <string>

using byte = unsigned char;

enum class Instruction : byte
{
    Integer_Add = 0,
    Integer_Subtract,
    Integer_Multiply,
    Integer_Divide,
    Integer_Modulo,
    Integer_And,
    Integer_Or,
    Integer_Xor,
    Integer_Left_Shift,
    Integer_Right_Shift,
    Integer_Greater_Than,
    Integer_Greater_Than_Equals,
    Integer_Less_Than,
    Integer_Less_Than_Equals,
    Integer_Negate,
    Integer_Invert,

    Noop,

    Literal_1,
    Literal_2,
    Literal_4,
    Literal_int,

    Get_Value,
    Set_Value,

    Get_Type,

    Branch,
    Branch_If_Zero,

    Call_Method,
    Call_Method_Cast_To,
    Call_Method_Add,
    Call_Method_Subtract,
    Call_Method_Multiply,
    Call_Method_Divide,
    Call_Method_Modulo,
    Call_Method_And,
    Call_Method_Or,
    Call_Method_Xor,
    Call_Method_Left_Shift,
    Call_Method_Right_Shift,
    Call_Method_Greater_Than,
    Call_Method_Greater_Than_Equals,
    Call_Method_Less_Than,
    Call_Method_Less_Than_Equals,
    Call_Method_Negate,
    Call_Method_Invert,
    Call_Method_Not,
    Call_Method_To_String,


    INSTRUCTION_ENUM_SIZE
};

constexpr const char *instruction_to_string(const Instruction i)
{
    switch (i)
    {
        case Instruction::Integer_Add: return "Integer_Add";
        case Instruction::Integer_Subtract: return "Integer_Subtract";
        case Instruction::Integer_Multiply: return "Integer_Multiply";
        case Instruction::Integer_Divide: return "Integer_Divide";
        case Instruction::Integer_Modulo: return "Integer_Modulo";
        case Instruction::Integer_And: return "Integer_And";
        case Instruction::Integer_Or: return "Integer_Or";
        case Instruction::Integer_Xor: return "Integer_Xor";
        case Instruction::Integer_Left_Shift: return "Integer_Left_Shift";
        case Instruction::Integer_Right_Shift: return "Integer_Right_Shift";
        case Instruction::Integer_Greater_Than: return "Integer_Greater_Than";
        case Instruction::Integer_Greater_Than_Equals: return "Integer_Greater_Than_Equals";
        case Instruction::Integer_Less_Than: return "Integer_Less_Than";
        case Instruction::Integer_Less_Than_Equals: return "Integer_Less_Than_Equals";
        case Instruction::Integer_Negate: return "Integer_Negate";
        case Instruction::Integer_Invert: return "Integer_Invert";
        case Instruction::Noop: return "Noop";
        case Instruction::Literal_1: return "Literal_1:";
        case Instruction::Literal_2: return "Literal_2:";
        case Instruction::Literal_4: return "Literal_4:";
        case Instruction::Literal_int: return "Literal_int";
        case Instruction::Get_Value: return "Get_Value";
        case Instruction::Set_Value: return "Set_Value";
        case Instruction::Get_Type: return "Get_Type";
        case Instruction::Branch: return "Branch";
        case Instruction::Branch_If_Zero: return "Branch_If_Zero";
        case Instruction::Call_Method: return "Call_Method";
        case Instruction::Call_Method_Cast_To: return "Call_Method_Cast_To";
        case Instruction::Call_Method_Add: return "Call_Method_Add";
        case Instruction::Call_Method_Subtract: return "Call_Method_Subtract";
        case Instruction::Call_Method_Multiply: return "Call_Method_Multiply";
        case Instruction::Call_Method_Divide: return "Call_Method_Divide";
        case Instruction::Call_Method_Modulo: return "Call_Method_Modulo";
        case Instruction::Call_Method_And: return "Call_Method_And";
        case Instruction::Call_Method_Or: return "Call_Method_Or";
        case Instruction::Call_Method_Xor: return "Call_Method_Xor";
        case Instruction::Call_Method_Left_Shift: return "Call_Method_Left_Shift";
        case Instruction::Call_Method_Right_Shift: return "Call_Method_Right_Shift";
        case Instruction::Call_Method_Greater_Than: return "Call_Method_Greater_Than";
        case Instruction::Call_Method_Greater_Than_Equals: return "Call_Method_Greater_Than_Equals";
        case Instruction::Call_Method_Less_Than: return "Call_Method_Less_Than";
        case Instruction::Call_Method_Less_Than_Equals: return "Call_Method_Less_Than_Equals";
        case Instruction::Call_Method_Negate: return "Call_Method_Negate";
        case Instruction::Call_Method_Invert: return "Call_Method_Invert";
        case Instruction::Call_Method_Not: return "Call_Method_Not";
        case Instruction::Call_Method_To_String: return "Call_Method_To_String";
        case Instruction::INSTRUCTION_ENUM_SIZE : return "INSTRUCTION_ENUM_SIZE";
    }
}

#endif /* MALANG_VM_INSTRUCTION_HPP */ 
