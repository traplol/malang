#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <stdint.h>
#include "vm.hpp"
#include "instruction.hpp"

void Malang_VM::load_code(std::vector<byte> code)
{
    this->code.clear();
    this->code.insert(this->code.begin(), code.begin(), code.end());
}

static void run_code(Malang_VM&);
void Malang_VM::run()
{
    ip = code.data();
    data_stack.clear();
    return_stack.clear();

    run_code(*this);
}


static void vprint(const char *fmt, va_list vargs)
{
    vprintf(fmt, vargs);
}

static void print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint(fmt, args);
    va_end(args);
}

static void trace(Malang_VM &vm)
{
    auto code_start = vm.code.data();
    auto code_end = vm.code.data() + vm.code.size();
    auto x = std::min(16l, vm.ip - code_start);
    auto y = std::min(16l, code_end - vm.ip);

    auto start = vm.ip - x;
    auto end = vm.ip + y;
    auto p = start;
    for (int i = 1; p != end; ++p, ++i)
    {
        printf("%x ", static_cast<int>(*p));
        if (i % 16 == 0)
        {
            print("\n");
        }
    }
}

void trace_abort(Malang_VM &vm, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint(fmt, args);
    va_end(args);
    trace(vm);
    abort();
}

#define NOT_IMPL {trace_abort(vm, "%s:%d `%s()` not implemented\n", __FILE__, __LINE__, __FUNCTION__);}

static inline byte fetch1(byte *p)
{
    return *p;
}

static inline int16_t fetch2(byte *p)
{
    return *reinterpret_cast<int16_t*>(p);
}

static inline int32_t fetch4(byte *p)
{
    return *reinterpret_cast<int32_t*>(p);
}

static inline intptr_t fetch_int(byte *p)
{
    return *reinterpret_cast<intptr_t*>(p);
}

#define POP() vm.data_stack.back(); vm.data_stack.pop_back()
#define PUSH(x) vm.data_stack.push_back(x)
#define PEEK() vm.data_stack.back();
#define NEXT1 vm.ip += 1
#define NEXT2 vm.ip += 2
#define NEXT4 vm.ip += 4
#define NEXTPTR vm.ip += sizeof(intptr_t)

static inline void exec_integer_add(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a + b);
    NEXT1;
}

static inline void exec_integer_subract(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a - b);
    NEXT1;
}

static inline void exec_integer_multiply(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a * b);
    NEXT1;
}

static inline void exec_integer_divide(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a / b);
    NEXT1;
}

static inline void exec_integer_modulo(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a % b);
    NEXT1;
}

static inline void exec_integer_and(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a & b);
    NEXT1;
}

static inline void exec_integer_or(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a | b);
    NEXT1;
}

static inline void exec_integer_xor(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a ^ b);
    NEXT1;
}

static inline void exec_integer_left_shift(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a << b);
    NEXT1;
}

static inline void exec_integer_right_shift(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a >> b);
    NEXT1;
}

static inline void exec_integer_greater_than(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a > b);
    NEXT1;
}

static inline void exec_integer_greater_than_equals(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a >= b);
    NEXT1;
}

static inline void exec_integer_less_than(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a < b);
    NEXT1;
}

static inline void exec_integer_less_than_equals(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a <= b);
    NEXT1;
}

static inline void exec_integer_negate(Malang_VM &vm)
{
    auto a = POP();
    PUSH(-a);
    NEXT1;
}

static inline void exec_integer_invert(Malang_VM &vm)
{
    auto a = POP();
    PUSH(~a);
    NEXT1;
}

static inline void exec_noop(Malang_VM &vm)
{
    NEXT1;
}

static inline void exec_literal_1(Malang_VM &vm)
{
    NEXT1;
    auto one = fetch1(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(one));
    NEXT1;
}

static inline void exec_literal_2(Malang_VM &vm)
{
    NEXT1;
    auto two = fetch2(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(two));
    NEXT2;
}

static inline void exec_literal_4(Malang_VM &vm)
{
    NEXT1;
    auto four = fetch4(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(four));
    NEXT4;
}

static inline void exec_literal_int(Malang_VM &vm)
{
    NEXT1;
    auto ptr = fetch_int(vm.ip);
    vm.data_stack.push_back(ptr);
    NEXTPTR;
}

static inline void exec_get_value(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_set_value(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_get_type(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_branch(Malang_VM &vm)
{
    NEXT1;
    auto n = fetch_int(vm.ip);
    vm.ip += n; // XXX: n-1 to be relative to the branch instruction
}

static inline void exec_branch_if_zero(Malang_VM &vm)
{
    NEXT1;
    auto cond = POP();
    if (cond == 0)
    {
        auto n = fetch_int(vm.ip);
        vm.ip += n; // XXX: n-1 to be relative to the branch instruction
    }
    else
    { // need to skip the offset
        NEXTPTR;
    }
}

static inline void exec_call_method(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_cast_to(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_add(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_subtract(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_multiply(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_divide(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_modulo(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_and(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_or(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_xor(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_left_shift(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_right_shift(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_greater_than(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_greater_than_equals(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_less_than(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_less_than_equals(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_negate(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_invert(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_not(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_call_method_to_string(Malang_VM &vm)
{
    NOT_IMPL;
}

static void run_code(Malang_VM &vm)
{
    auto end = &vm.code[vm.code.size()];
    while (vm.ip != end)
    {
        auto ins = static_cast<Instruction>(fetch1(vm.ip));
        // We use continue instead of break so the compiler will warn us when
        // we forget to add an instruction to our dispatch and so an out-of-
        // range case will cause a trace_abort
        switch (ins)
        {
            case Instruction::Integer_Add:
                exec_integer_add(vm);
                continue;
            case Instruction::Integer_Subtract:
                exec_integer_subract(vm);
                continue;
            case Instruction::Integer_Multiply:
                exec_integer_multiply(vm);
                continue;
            case Instruction::Integer_Divide:
                exec_integer_divide(vm);
                continue;
            case Instruction::Integer_Modulo:
                exec_integer_modulo(vm);
                continue;
            case Instruction::Integer_And:
                exec_integer_and(vm);
                continue;
            case Instruction::Integer_Or:
                exec_integer_or(vm);
                continue;
            case Instruction::Integer_Xor:
                exec_integer_xor(vm);
                continue;
            case Instruction::Integer_Left_Shift:
                exec_integer_left_shift(vm);
                continue;
            case Instruction::Integer_Right_Shift:
                exec_integer_right_shift(vm);
                continue;
            case Instruction::Integer_Greater_Than:
                exec_integer_greater_than(vm);
                continue;
            case Instruction::Integer_Greater_Than_Equals:
                exec_integer_greater_than_equals(vm);
                continue;
            case Instruction::Integer_Less_Than:
                exec_integer_less_than(vm);
                continue;
            case Instruction::Integer_Less_Than_Equals:
                exec_integer_less_than_equals(vm);
                continue;
            case Instruction::Integer_Negate:
                exec_integer_negate(vm);
                continue;
            case Instruction::Integer_Invert:
                exec_integer_invert(vm);
                continue;
            case Instruction::Noop:
                exec_noop(vm);
                continue;
            case Instruction::Literal_1:
                exec_literal_1(vm);
                continue;
            case Instruction::Literal_2:
                exec_literal_2(vm);
                continue;
            case Instruction::Literal_4:
                exec_literal_4(vm);
                continue;
            case Instruction::Literal_int:
                exec_literal_int(vm);
                continue;
            case Instruction::Get_Value:
                exec_get_value(vm);
                continue;
            case Instruction::Set_Value:
                exec_set_value(vm);
                continue;
            case Instruction::Get_Type:
                exec_get_type(vm);
                continue;
            case Instruction::Branch:
                exec_branch(vm);
                continue;
            case Instruction::Branch_If_Zero:
                exec_branch_if_zero(vm);
                continue;
            case Instruction::Call_Method:
                exec_call_method(vm);
                continue;
            case Instruction::Call_Method_Cast_To:
                exec_call_method_cast_to(vm);
                continue;
            case Instruction::Call_Method_Add:
                exec_call_method_add(vm);
                continue;
            case Instruction::Call_Method_Subtract:
                exec_call_method_subtract(vm);
                continue;
            case Instruction::Call_Method_Multiply:
                exec_call_method_multiply(vm);
                continue;
            case Instruction::Call_Method_Divide:
                exec_call_method_divide(vm);
                continue;
            case Instruction::Call_Method_Modulo:
                exec_call_method_modulo(vm);
                continue;
            case Instruction::Call_Method_And:
                exec_call_method_and(vm);
                continue;
            case Instruction::Call_Method_Or:
                exec_call_method_or(vm);
                continue;
            case Instruction::Call_Method_Xor:
                exec_call_method_xor(vm);
                continue;
            case Instruction::Call_Method_Left_Shift:
                exec_call_method_left_shift(vm);
                continue;
            case Instruction::Call_Method_Right_Shift:
                exec_call_method_right_shift(vm);
                continue;
            case Instruction::Call_Method_Greater_Than:
                exec_call_method_greater_than(vm);
                continue;
            case Instruction::Call_Method_Greater_Than_Equals:
                exec_call_method_greater_than_equals(vm);
                continue;
            case Instruction::Call_Method_Less_Than:
                exec_call_method_less_than(vm);
                continue;
            case Instruction::Call_Method_Less_Than_Equals:
                exec_call_method_less_than_equals(vm);
                continue;
            case Instruction::Call_Method_Negate:
                exec_call_method_negate(vm);
                continue;
            case Instruction::Call_Method_Invert:
                exec_call_method_invert(vm);
                continue;
            case Instruction::Call_Method_Not:
                exec_call_method_not(vm);
                continue;
            case Instruction::Call_Method_To_String:
                exec_call_method_to_string(vm);
                continue;
            case Instruction::INSTRUCTION_ENUM_SIZE:
                break;
        }
        trace_abort(vm, "Unknown instruction: %x\n", static_cast<int>(ins));
    }
}
