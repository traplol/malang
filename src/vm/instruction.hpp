#ifndef MALANG_VM_INSTRUCTION_HPP
#define MALANG_VM_INSTRUCTION_HPP

using byte = unsigned char;

enum Instruction : byte
{
    Ins_Integer_Add = 0,
    Ins_Integer_Subtract,
    Ins_Integer_Multiply,
    Ins_Integer_Divide,
    Ins_Integer_Modulo,
    Ins_Integer_And,
    Ins_Integer_Or,
    Ins_Integer_Xor,
    Ins_Integer_Left_Shift,
    Ins_Integer_Right_Shift,
    Ins_Integer_Greater_Than,
    Ins_Integer_Greater_Than_Equals,
    Ins_Integer_Less_Than,
    Ins_Integer_Less_Than_Equals,
    Ins_Integer_Negate,
    Ins_Integer_Invert,

    Ins_Noop,

    Ins_Literal_1,
    Ins_Literal_2,
    Ins_Literal_4,
    Ins_Literal_ptr,

    Ins_Get_Value,
    Ins_Set_Value,

    Ins_Get_Type,

    Ins_Branch,
    Ins_Branch_If_Zero,

    Ins_Call_Method,
    Ins_Call_Method_Cast_To,
    Ins_Call_Method_Add,
    Ins_Call_Method_Subtract,
    Ins_Call_Method_Multiply,
    Ins_Call_Method_Divide,
    Ins_Call_Method_Modulo,
    Ins_Call_Method_And,
    Ins_Call_Method_Or,
    Ins_Call_Method_Xor,
    Ins_Call_Method_Left_Shift,
    Ins_Call_Method_Right_Shift,
    Ins_Call_Method_Greater_Than,
    Ins_Call_Method_Greater_Than_Equals,
    Ins_Call_Method_Less_Than,
    Ins_Call_Method_Less_Than_Equals,
    Ins_Call_Method_Negate,
    Ins_Call_Method_Invert,
    Ins_Call_Method_Not,
    Ins_Call_Method_To_String,


    INSTRUCTION_ENUM_SIZE
};

static_assert(Instruction::INSTRUCTION_ENUM_SIZE < 256, "Too many instructions?...");

#endif /* MALANG_VM_INSTRUCTION_HPP */
