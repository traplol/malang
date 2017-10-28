#ifndef MALANG_CODEGEN_CODEGEN_HPP
#define MALANG_CODEGEN_CODEGEN_HPP

#include "../vm/vm.hpp"
#include "../vm/instruction.hpp"
#include "../vm/runtime/primitive_types.hpp"

struct Codegen
{
    std::vector<byte> code;

    void push_back_instruction(Instruction instruction);
    void push_back_halt();
    
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
    void push_back_fixnum_not_equals();
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
    void push_back_fixnum_not_equals(Fixnum a, Fixnum b);
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
    void push_back_literal_double(Double n);
    void push_back_literal_value(Malang_Value value);
    void push_back_raw_8(byte n);
    void push_back_raw_16(int16_t n);
    void push_back_raw_32(int32_t n);

    void push_back_call_primitive(const Primitive_Function &primitive);
    void push_back_call_primitive(int32_t index);
    void push_back_call_primitive_dyn(const Primitive_Function &primitive);
    void push_back_call_primitive_dyn(int32_t index);
    void push_back_call_primitive_dyn();
    void push_back_call_code(int32_t code);
    void push_back_call_code();
    void push_back_call_code_dyn(int32_t code);
    void push_back_call_code_dyn();
    void push_back_return(bool fast);

    void push_back_store_local(uint16_t n);
    void push_back_store_global(uint32_t n);
    void push_back_load_local(uint16_t n);
    void push_back_load_global(uint32_t n);

    void push_back_dup_1();
    void push_back_dup_2();
    void push_back_swap();
    void push_back_over();
    void push_back_drop(uint16_t n);

    // Returns the index into the code where the dummy value is.
    size_t push_back_branch();
    size_t push_back_pop_branch_if_false();
    size_t push_back_pop_branch_if_true();
    size_t push_back_branch_if_false_or_pop();
    size_t push_back_branch_if_true_or_pop();
    void push_back_branch(int32_t n);
    void push_back_pop_branch_if_false(int32_t n);
    void push_back_pop_branch_if_true(int32_t n);
    void push_back_branch_if_false_or_pop(int32_t n);
    void push_back_branch_if_true_or_pop(int32_t n);

    void set_raw_8(size_t index, byte value);
    void set_raw_16(size_t index, int16_t value);
    void set_raw_32(size_t index, int32_t value);

    void push_back_alloc_locals(uint16_t num_to_alloc);

    void push_back_array_new(Type_Token type, int32_t length);
    void push_back_array_new(Type_Token type);
    void push_back_array_load(bool checked = true);
    void push_back_array_store(bool checked = true);
    void push_back_array_length();

    void push_back_buffer_new();
    void push_back_buffer_new(int32_t length);
    void push_back_buffer_copy();
    void push_back_buffer_load(bool checked = true);
    void push_back_buffer_store(bool checked = true);
    void push_back_buffer_length();
    void push_back_load_string_constant(int32_t index);

};

#endif /* MALANG_CODEGEN_CODEGEN_HPP */
