#include <string.h>
#include "codegen.hpp"

void Codegen::push_back_instruction(Instruction instruction)
{
    code.push_back(static_cast<byte>(instruction));
}
void Codegen::push_back_fixnum_add()
{
    push_back_instruction(Instruction::Fixnum_Add);
}
void Codegen::push_back_fixnum_subtract()
{
    push_back_instruction(Instruction::Fixnum_Subtract);
}
void Codegen::push_back_fixnum_multiply()
{
    push_back_instruction(Instruction::Fixnum_Multiply);
}
void Codegen::push_back_fixnum_divide()
{
    push_back_instruction(Instruction::Fixnum_Divide);
}
void Codegen::push_back_fixnum_modulo()
{
    push_back_instruction(Instruction::Fixnum_Modulo);
}
void Codegen::push_back_fixnum_and()
{
    push_back_instruction(Instruction::Fixnum_And);
}
void Codegen::push_back_fixnum_or()
{
    push_back_instruction(Instruction::Fixnum_Or);
}
void Codegen::push_back_fixnum_xor()
{
    push_back_instruction(Instruction::Fixnum_Xor);
}
void Codegen::push_back_fixnum_left_shift()
{
    push_back_instruction(Instruction::Fixnum_Left_Shift);
}
void Codegen::push_back_fixnum_right_shift()
{
    push_back_instruction(Instruction::Fixnum_Right_Shift);
}
void Codegen::push_back_fixnum_equals()
{
    push_back_instruction(Instruction::Fixnum_Equals);
}
void Codegen::push_back_fixnum_greater_than()
{
    push_back_instruction(Instruction::Fixnum_Greater_Than);
}
void Codegen::push_back_fixnum_greater_than_equals()
{
    push_back_instruction(Instruction::Fixnum_Greater_Than_Equals);
}
void Codegen::push_back_fixnum_less_than()
{
    push_back_instruction(Instruction::Fixnum_Less_Than);
}
void Codegen::push_back_fixnum_less_than_equals()
{
    push_back_instruction(Instruction::Fixnum_Less_Than_Equals);
}
void Codegen::push_back_fixnum_negate()
{
    push_back_instruction(Instruction::Fixnum_Negate);
}
void Codegen::push_back_fixnum_invert()
{
    push_back_instruction(Instruction::Fixnum_Invert);
}
void Codegen::push_back_fixnum_add(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_add();
}
void Codegen::push_back_fixnum_subtract(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_subtract();
}
void Codegen::push_back_fixnum_multiply(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_multiply();
}
void Codegen::push_back_fixnum_divide(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_multiply();
}
void Codegen::push_back_fixnum_modulo(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_modulo();
}
void Codegen::push_back_fixnum_and(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_and();
}
void Codegen::push_back_fixnum_or(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_or();
}
void Codegen::push_back_fixnum_xor(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_xor();
}
void Codegen::push_back_fixnum_left_shift(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_left_shift();
}
void Codegen::push_back_fixnum_right_shift(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_right_shift();
}
void Codegen::push_back_fixnum_greater_than(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_greater_than();
}
void Codegen::push_back_fixnum_greater_than_equals(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_greater_than_equals();
}
void Codegen::push_back_fixnum_less_than(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_less_than();
}
void Codegen::push_back_fixnum_less_than_equals(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_less_than_equals();
}
void Codegen::push_back_fixnum_negate(Fixnum n)
{
    push_back_literal_value(n);
    push_back_fixnum_negate();
}
void Codegen::push_back_fixnum_invert(Fixnum n)
{
    push_back_literal_value(n);
    push_back_fixnum_invert();
}
void Codegen::push_back_noop()
{
    push_back_instruction(Instruction::Noop);
}
void Codegen::push_back_literal_8(byte n)
{
    push_back_instruction(Instruction::Literal_8);
    push_back_raw_8(n);
}
void Codegen::push_back_literal_16(int16_t n)
{
    push_back_instruction(Instruction::Literal_16);
    push_back_raw_16(n);
}
void Codegen::push_back_literal_32(int32_t n)
{
    push_back_instruction(Instruction::Literal_32);
    push_back_raw_32(n);
}
void Codegen::push_back_literal_value(Malang_Value value)
{
    push_back_instruction(Instruction::Literal_value);
    code.resize(code.size() + sizeof(value));
    auto end = code.data() + code.size();
    auto slot = reinterpret_cast<decltype(value)*>(end - sizeof(value));
    *slot = value;
}
void Codegen::push_back_raw_8(byte n)
{
    code.push_back(n);
}
void Codegen::push_back_raw_16(int16_t n)
{
    code.resize(code.size() + sizeof(n));
    auto end = code.data() + code.size();
    auto slot = reinterpret_cast<decltype(n)*>(end - sizeof(n));
    *slot = n;
}
void Codegen::push_back_raw_32(int32_t n)
{
    code.resize(code.size() + sizeof(n));
    auto end = code.data() + code.size();
    auto slot = reinterpret_cast<decltype(n)*>(end - sizeof(n));
    *slot = n;
}

void Codegen::push_back_call_primitive(const Primitive_Function &primitive)
{
    push_back_instruction(Instruction::Call_Primitive);
    push_back_raw_32(primitive.index);
}
void Codegen::push_back_call_code(int32_t code)
{
    push_back_literal_32(code);
    push_back_instruction(Instruction::Call);
}
