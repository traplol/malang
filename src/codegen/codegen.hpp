#ifndef MALANG_CODEGEN_CODEGEN_HPP
#define MALANG_CODEGEN_CODEGEN_HPP

#include "../vm/vm.hpp"
#include "../vm/instruction.hpp"
#include "../vm/runtime_primitive_types.hpp"

struct Codegen
{
    std::vector<byte> code;

    void push_back_instruction(Instruction instruction);
    
    void push_back_fixnum_add();
    void push_back_fixnum_subtract();
    void push_back_fixnum_multiply();
    void push_back_fixnum_divide();
    void push_back_fixnum_modulo();
    void push_back_fixnum_and();
    void push_back_fixnum_or();
    void push_back_fixnum_xor();
    void push_back_fixnum_left_shift();
    void push_back_fixnum_right_shift();
    void push_back_fixnum_equals();
    void push_back_fixnum_greater_than();
    void push_back_fixnum_greater_than_equals();
    void push_back_fixnum_less_than();
    void push_back_fixnum_less_than_equals();
    void push_back_fixnum_negate();
    void push_back_fixnum_invert();

    void push_back_fixnum_add(Fixnum a, Fixnum b);
    void push_back_fixnum_subtract(Fixnum a, Fixnum b);
    void push_back_fixnum_multiply(Fixnum a, Fixnum b);
    void push_back_fixnum_divide(Fixnum a, Fixnum b);
    void push_back_fixnum_modulo(Fixnum a, Fixnum b);
    void push_back_fixnum_and(Fixnum a, Fixnum b);
    void push_back_fixnum_or(Fixnum a, Fixnum b);
    void push_back_fixnum_xor(Fixnum a, Fixnum b);
    void push_back_fixnum_left_shift(Fixnum a, Fixnum b);
    void push_back_fixnum_right_shift(Fixnum a, Fixnum b);
    void push_back_fixnum_equals(Fixnum a, Fixnum b);
    void push_back_fixnum_greater_than(Fixnum a, Fixnum b);
    void push_back_fixnum_greater_than_equals(Fixnum a, Fixnum b);
    void push_back_fixnum_less_than(Fixnum a, Fixnum b);
    void push_back_fixnum_less_than_equals(Fixnum a, Fixnum b);
    void push_back_fixnum_negate(Fixnum n);
    void push_back_fixnum_invert(Fixnum n);

    void push_back_noop();

    void push_back_literal_8(byte n);
    void push_back_literal_16(int16_t n);
    void push_back_literal_32(int32_t n);
    void push_back_literal_value(Malang_Value value);

    void push_back_call_method_cast_to();
    void push_back_call_method_add();
    void push_back_call_method_subtract();
    void push_back_call_method_multiply();
    void push_back_call_method_divide();
    void push_back_call_method_modulo();
    void push_back_call_method_and();
    void push_back_call_method_or();
    void push_back_call_method_xor();
    void push_back_call_method_left_shift();
    void push_back_call_method_right_shift();
    void push_back_call_method_equals();
    void push_back_call_method_greater_than();
    void push_back_call_method_greater_than_equals();
    void push_back_call_method_less_than();
    void push_back_call_method_less_than_equals();
    void push_back_call_method_negate();
    void push_back_call_method_invert();
    void push_back_call_method_not();
    void push_back_call_method_to_string();
};

#endif /* MALANG_CODEGEN_CODEGEN_HPP */
