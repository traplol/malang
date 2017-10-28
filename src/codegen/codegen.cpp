#include <string.h>
#include "codegen.hpp"

void Codegen::push_back_instruction(Instruction instruction)
{
    code.push_back(static_cast<byte>(instruction));
}
void Codegen::push_back_halt()
{
    push_back_instruction(Instruction::Halt);
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
void Codegen::push_back_fixnum_not_equals()
{
    push_back_instruction(Instruction::Fixnum_Not_Equals);
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
void Codegen::push_back_fixnum_equals(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_equals();
}
void Codegen::push_back_fixnum_not_equals(Fixnum a, Fixnum b)
{
    push_back_literal_value(a);
    push_back_literal_value(b);
    push_back_fixnum_not_equals();
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
static inline
bool push_back_fast_constant(Codegen &cg, int32_t n)
{
    switch (n)
    {
        case -1:
            cg.push_back_instruction(Instruction::Literal_Fixnum_m1);
            return true;
        case 0:
            cg.push_back_instruction(Instruction::Literal_Fixnum_0);
            return true;
        case 1:
            cg.push_back_instruction(Instruction::Literal_Fixnum_1);
            return true;
        case 2:
            cg.push_back_instruction(Instruction::Literal_Fixnum_2);
            return true;
        case 3:
            cg.push_back_instruction(Instruction::Literal_Fixnum_3);
            return true;
        case 4:
            cg.push_back_instruction(Instruction::Literal_Fixnum_4);
            return true;
        case 5:
            cg.push_back_instruction(Instruction::Literal_Fixnum_5);
            return true;
    }
    return false;
}
void Codegen::push_back_literal_8(byte n)
{
    if (!push_back_fast_constant(*this, n))
    {
        push_back_instruction(Instruction::Literal_8);
        push_back_raw_8(n);
    }
}
void Codegen::push_back_literal_16(int16_t n)
{
    if (!push_back_fast_constant(*this, n))
    {
        push_back_instruction(Instruction::Literal_16);
        push_back_raw_16(n);
    }
}
void Codegen::push_back_literal_32(int32_t n)
{
    if (!push_back_fast_constant(*this, n))
    {
        push_back_instruction(Instruction::Literal_32);
        push_back_raw_32(n);
    }
}
void Codegen::push_back_literal_double(Double n)
{
    if (sizeof(uint64_t) != sizeof(Double))
    {
        push_back_literal_value(n);
        return;
    }

    union {Double d; uint64_t i;} zero, one, two, _n;
    zero.d = 0.0;
    one.d = 1.0;
    two.d = 2.0;
    _n.d = n;


    if (_n.i == zero.i)
    {
        push_back_instruction(Instruction::Literal_Double_0);
    }
    else if (_n.i == one.i)
    {
        push_back_instruction(Instruction::Literal_Double_1);
    }
    else if (_n.i == two.i)
    {
        push_back_instruction(Instruction::Literal_Double_2);
    }
    else
    {
        push_back_literal_value(n);
    }
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
    push_back_call_primitive(primitive.index);
}
void Codegen::push_back_call_primitive(int32_t index)
{
    push_back_instruction(Instruction::Call_Primitive);
    push_back_raw_32(index);
}
void Codegen::push_back_call_primitive_dyn(const Primitive_Function &primitive)
{
    push_back_call_primitive_dyn(primitive.index);
}
void Codegen::push_back_call_primitive_dyn(int32_t index)
{
    push_back_literal_32(index);
    push_back_call_primitive_dyn();
}
void Codegen::push_back_call_primitive_dyn()
{
    push_back_instruction(Instruction::Call_Primitive_Dyn);
}
void Codegen::push_back_call_code(int32_t code)
{
    push_back_call_code();
    push_back_raw_32(code);
}
void Codegen::push_back_call_code()
{
    push_back_instruction(Instruction::Call);
}
void Codegen::push_back_call_code_dyn(int32_t code)
{
    push_back_literal_32(code);
    push_back_call_code_dyn();
}
void Codegen::push_back_call_code_dyn()
{
    push_back_instruction(Instruction::Call_Dyn);
}
void Codegen::push_back_return(bool fast)
{
    if (fast)
    {
        push_back_instruction(Instruction::Return_Fast);
    }
    else
    {
        push_back_instruction(Instruction::Return);
    }
}

void Codegen::push_back_store_local(uint16_t n)
{
    switch (n)
    {
        case 0:
            push_back_instruction(Instruction::Store_Local_0);
            break;
        case 1:
            push_back_instruction(Instruction::Store_Local_1);
            break;
        case 2:
            push_back_instruction(Instruction::Store_Local_2);
            break;
        case 3:
            push_back_instruction(Instruction::Store_Local_3);
            break;
        case 4:
            push_back_instruction(Instruction::Store_Local_4);
            break;
        case 5:
            push_back_instruction(Instruction::Store_Local_5);
            break;
        case 6:
            push_back_instruction(Instruction::Store_Local_6);
            break;
        case 7:
            push_back_instruction(Instruction::Store_Local_7);
            break;
        case 8:
            push_back_instruction(Instruction::Store_Local_8);
            break;
        case 9:
            push_back_instruction(Instruction::Store_Local_9);
            break;
        default:
            push_back_instruction(Instruction::Store_Local);
            push_back_raw_16(n);
            break;
    }
}
void Codegen::push_back_load_local(uint16_t n)
{
    switch (n)
    {
        case 0:
            push_back_instruction(Instruction::Load_Local_0);
            break;
        case 1:
            push_back_instruction(Instruction::Load_Local_1);
            break;
        case 2:
            push_back_instruction(Instruction::Load_Local_2);
            break;
        case 3:
            push_back_instruction(Instruction::Load_Local_3);
            break;
        case 4:
            push_back_instruction(Instruction::Load_Local_4);
            break;
        case 5:
            push_back_instruction(Instruction::Load_Local_5);
            break;
        case 6:
            push_back_instruction(Instruction::Load_Local_6);
            break;
        case 7:
            push_back_instruction(Instruction::Load_Local_7);
            break;
        case 8:
            push_back_instruction(Instruction::Load_Local_8);
            break;
        case 9:
            push_back_instruction(Instruction::Load_Local_9);
            break;
        default:
            push_back_instruction(Instruction::Load_Local);
            push_back_raw_16(n);
            break;
    }
}

void Codegen::push_back_store_global(uint32_t n)
{
    push_back_instruction(Instruction::Store_Global);
    push_back_raw_32(n);
}
void Codegen::push_back_load_global(uint32_t n)
{
    push_back_instruction(Instruction::Load_Global);
    push_back_raw_32(n);
}

void Codegen::push_back_dup_1()
{
    push_back_instruction(Instruction::Dup_1);
}
void Codegen::push_back_dup_2()
{
    push_back_instruction(Instruction::Dup_2);
}
void Codegen::push_back_swap()
{
    push_back_instruction(Instruction::Swap_1);
}
void Codegen::push_back_over()
{
    push_back_instruction(Instruction::Over_1);
}
void Codegen::push_back_drop(uint16_t n)
{
    if (n == 0) return;
    switch (n)
    {
        case 1:
            push_back_instruction(Instruction::Drop_1);
            break;
        case 2:
            push_back_instruction(Instruction::Drop_2);
            break;
        case 3:
            push_back_instruction(Instruction::Drop_3);
            break;
        case 4:
            push_back_instruction(Instruction::Drop_4);
            break;
        default:
            push_back_instruction(Instruction::Drop_N);
            push_back_raw_16(n);
            break;
    }
}

size_t Codegen::push_back_branch()
{
    push_back_instruction(Instruction::Branch);
    auto idx = code.size();
    push_back_raw_32(0);
    return idx;
}
size_t Codegen::push_back_branch_if_zero(bool pop)
{
    if (pop)
    {
        push_back_instruction(Instruction::Pop_Branch_If_Zero);
    }
    else
    {
        push_back_instruction(Instruction::Branch_If_Zero);
    }
    auto idx = code.size();
    push_back_raw_32(0);
    return idx;
}
size_t Codegen::push_back_branch_if_not_zero(bool pop)
{
    if (pop)
    {
        push_back_instruction(Instruction::Pop_Branch_If_Not_Zero);
    }
    else
    {
        push_back_instruction(Instruction::Branch_If_Not_Zero);
    }
    auto idx = code.size();
    push_back_raw_32(0);
    return idx;
}
void Codegen::push_back_branch(int32_t n)
{
    push_back_instruction(Instruction::Branch);
    push_back_raw_32(n);
}
void Codegen::push_back_branch_if_zero(int32_t n, bool pop)
{
    if (pop)
    {
        push_back_instruction(Instruction::Pop_Branch_If_Zero);
    }
    else
    {
        push_back_instruction(Instruction::Branch_If_Zero);
    }
    push_back_raw_32(n);
}
void Codegen::push_back_branch_if_not_zero(int32_t n, bool pop)
{
    if (pop)
    {
        push_back_instruction(Instruction::Pop_Branch_If_Not_Zero);
    }
    else
    {
        push_back_instruction(Instruction::Branch_If_Not_Zero);
    }
    push_back_raw_32(n);
}

void Codegen::set_raw_8(size_t index, byte value)
{
    assert(index+sizeof(value)-1 < code.size());
    auto slot = code.data()+index;
    *slot = value;
}
void Codegen::set_raw_16(size_t index, int16_t value)
{
    assert(index+sizeof(value)-1 < code.size());
    auto slot = code.data()+index;
    *reinterpret_cast<decltype(value)*>(slot) = value;
}
void Codegen::set_raw_32(size_t index, int32_t value)
{
    assert(index+sizeof(value)-1 < code.size());
    auto slot = code.data()+index;
    *reinterpret_cast<decltype(value)*>(slot) = value;
}

void Codegen::push_back_alloc_locals(uint16_t num_to_alloc)
{
    push_back_instruction(Instruction::Alloc_Locals);
    push_back_raw_16(num_to_alloc);
}

void Codegen::push_back_array_new(Type_Token type, int32_t length)
{
    push_back_literal_32(length);
    push_back_array_new(type);
}

void Codegen::push_back_array_new(Type_Token type)
{
    push_back_instruction(Instruction::Array_New);
    push_back_raw_32(type);
}

void Codegen::push_back_array_load(bool checked)
{
    if (checked)
    {
        push_back_instruction(Instruction::Array_Load_Checked);
    }
    else
    {
        push_back_instruction(Instruction::Array_Store_Checked);
    }
}

void Codegen::push_back_array_store(bool checked)
{
    if (checked)
    {
        push_back_instruction(Instruction::Array_Store_Checked);
    }
    else
    {
        push_back_instruction(Instruction::Array_Store_Checked);
    }
}

void Codegen::push_back_array_length()
{
    push_back_instruction(Instruction::Array_Length);
}

void Codegen::push_back_load_string_constant(int32_t index)
{
    push_back_instruction(Instruction::Load_String_Constant);
    push_back_raw_32(index);
}
