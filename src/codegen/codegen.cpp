#include <string.h>
#include "codegen.hpp"

void Codegen::push_back_instruction(Instruction instruction)
{
    code.push_back(static_cast<byte>(instruction));
}
void Codegen::push_back_fixnum_add()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Add));
}
void Codegen::push_back_fixnum_subtract()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Subtract));
}
void Codegen::push_back_fixnum_multiply()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Multiply));
}
void Codegen::push_back_fixnum_divide()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Divide));
}
void Codegen::push_back_fixnum_modulo()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Modulo));
}
void Codegen::push_back_fixnum_and()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_And));
}
void Codegen::push_back_fixnum_or()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Or));
}
void Codegen::push_back_fixnum_xor()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Xor));
}
void Codegen::push_back_fixnum_left_shift()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Left_Shift));
}
void Codegen::push_back_fixnum_right_shift()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Right_Shift));
}
void Codegen::push_back_fixnum_greater_than()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Greater_Than));
}
void Codegen::push_back_fixnum_greater_than_equals()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Greater_Than_Equals));
}
void Codegen::push_back_fixnum_less_than()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Less_Than_Equals));
}
void Codegen::push_back_fixnum_less_than_equals()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Less_Than_Equals));
}
void Codegen::push_back_fixnum_negate()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Negate));
}
void Codegen::push_back_fixnum_invert()
{
    code.push_back(static_cast<byte>(Instruction::Fixnum_Invert));
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
    code.push_back(static_cast<byte>(Instruction::Noop));
}
void Codegen::push_back_literal_8(byte n)
{
    code.push_back(static_cast<byte>(Instruction::Literal_8));
    code.push_back(n);
}
void Codegen::push_back_literal_16(int16_t n)
{
    code.push_back(static_cast<byte>(Instruction::Literal_16));
    code.resize(code.size() + sizeof(n));
    auto end = code.data() + code.size();
    auto slot = reinterpret_cast<decltype(n)*>(end - sizeof(n));
    *slot = n;
}
void Codegen::push_back_literal_32(int32_t n)
{
    code.push_back(static_cast<byte>(Instruction::Literal_32));
    code.resize(code.size() + sizeof(n));
    auto end = code.data() + code.size();
    auto slot = reinterpret_cast<decltype(n)*>(end - sizeof(n));
    *slot = n;
}
void Codegen::push_back_literal_value(Malang_Value value)
{
    code.push_back(static_cast<byte>(Instruction::Literal_value));
    code.resize(code.size() + sizeof(value));
    auto end = code.data() + code.size();
    auto slot = reinterpret_cast<decltype(value)*>(end - sizeof(value));
    *slot = value;
}
