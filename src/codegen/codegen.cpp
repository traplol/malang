#include <string.h>
#include "codegen.hpp"

void Codegen::push_back_integer_add()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Add));
}
void Codegen::push_back_integer_subtract()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Subtract));
}
void Codegen::push_back_integer_multiply()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Multiply));
}
void Codegen::push_back_integer_divide()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Divide));
}
void Codegen::push_back_integer_modulo()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Modulo));
}
void Codegen::push_back_integer_and()
{
    code.push_back(static_cast<byte>(Instruction::Integer_And));
}
void Codegen::push_back_integer_or()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Or));
}
void Codegen::push_back_integer_xor()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Xor));
}
void Codegen::push_back_integer_left_shift()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Left_Shift));
}
void Codegen::push_back_integer_right_shift()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Right_Shift));
}
void Codegen::push_back_integer_greater_than()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Greater_Than));
}
void Codegen::push_back_integer_greater_than_equals()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Greater_Than_Equals));
}
void Codegen::push_back_integer_less_than()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Less_Than_Equals));
}
void Codegen::push_back_integer_less_than_equals()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Less_Than_Equals));
}
void Codegen::push_back_integer_negate()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Negate));
}
void Codegen::push_back_integer_invert()
{
    code.push_back(static_cast<byte>(Instruction::Integer_Invert));
}
void Codegen::push_back_integer_add(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_add();
}
void Codegen::push_back_integer_subtract(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_subtract();
}
void Codegen::push_back_integer_multiply(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_multiply();
}
void Codegen::push_back_integer_divide(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_multiply();
}
void Codegen::push_back_integer_modulo(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_modulo();
}
void Codegen::push_back_integer_and(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_and();
}
void Codegen::push_back_integer_or(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_or();
}
void Codegen::push_back_integer_xor(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_xor();
}
void Codegen::push_back_integer_left_shift(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_left_shift();
}
void Codegen::push_back_integer_right_shift(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_right_shift();
}
void Codegen::push_back_integer_greater_than(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_greater_than();
}
void Codegen::push_back_integer_greater_than_equals(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_greater_than_equals();
}
void Codegen::push_back_integer_less_than(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_less_than();
}
void Codegen::push_back_integer_less_than_equals(Integer a, Integer b)
{
    push_back_literal_int(a);
    push_back_literal_int(b);
    push_back_integer_less_than_equals();
}
void Codegen::push_back_integer_negate(Integer n)
{
    push_back_literal_int(n);
    push_back_integer_negate();
}
void Codegen::push_back_integer_invert(Integer n)
{
    push_back_literal_int(n);
    push_back_integer_invert();
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
void Codegen::push_back_literal_int(intptr_t n)
{
    code.push_back(static_cast<byte>(Instruction::Literal_int));
    code.resize(code.size() + sizeof(n));
    auto end = code.data() + code.size();
    auto slot = reinterpret_cast<decltype(n)*>(end - sizeof(n));
    *slot = n;
}
