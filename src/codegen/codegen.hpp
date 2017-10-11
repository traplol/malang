#ifndef MALANG_CODEGEN_CODEGEN_HPP
#define MALANG_CODEGEN_CODEGEN_HPP

#include "../vm/vm.hpp"
#include "../vm/instruction.hpp"
#include "../vm/runtime_primitive_types.hpp"

struct Codegen
{
    std::vector<byte> code;
    
    void push_back_integer_add();
    void push_back_integer_subtract();
    void push_back_integer_multiply();
    void push_back_integer_divide();
    void push_back_integer_modulo();
    void push_back_integer_and();
    void push_back_integer_or();
    void push_back_integer_xor();
    void push_back_integer_left_shift();
    void push_back_integer_right_shift();
    void push_back_integer_greater_than();
    void push_back_integer_greater_than_equals();
    void push_back_integer_less_than();
    void push_back_integer_less_than_equals();
    void push_back_integer_negate();
    void push_back_integer_invert();

    void push_back_integer_add(Integer a, Integer b);
    void push_back_integer_subtract(Integer a, Integer b);
    void push_back_integer_multiply(Integer a, Integer b);
    void push_back_integer_divide(Integer a, Integer b);
    void push_back_integer_modulo(Integer a, Integer b);
    void push_back_integer_and(Integer a, Integer b);
    void push_back_integer_or(Integer a, Integer b);
    void push_back_integer_xor(Integer a, Integer b);
    void push_back_integer_left_shift(Integer a, Integer b);
    void push_back_integer_right_shift(Integer a, Integer b);
    void push_back_integer_greater_than(Integer a, Integer b);
    void push_back_integer_greater_than_equals(Integer a, Integer b);
    void push_back_integer_less_than(Integer a, Integer b);
    void push_back_integer_less_than_equals(Integer a, Integer b);
    void push_back_integer_negate(Integer n);
    void push_back_integer_invert(Integer n);

    void push_back_noop();

    void push_back_literal_1(byte n);
    void push_back_literal_2(int16_t n);
    void push_back_literal_4(int32_t n);
    void push_back_literal_int(intptr_t n);

    /*
    void push_back_get_value();
    void push_back_set_value();

    void push_back_get_type();

    void push_back_branch();
    void push_back_branch_if_zero();

    void push_back_call_method();
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
    void push_back_call_method_greater_than();
    void push_back_call_method_greater_than_equals();
    void push_back_call_method_less_than();
    void push_back_call_method_less_than_equals();
    void push_back_call_method_negate();
    void push_back_call_method_invert();
    void push_back_call_method_not();
    void push_back_call_method_to_string();
    */
};

#endif /* MALANG_CODEGEN_CODEGEN_HPP */
