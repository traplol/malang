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

static inline byte fetch8(byte *p)
{
    return *p;
}

static inline int16_t fetch16(byte *p)
{
    return *reinterpret_cast<int16_t*>(p);
}

static inline int32_t fetch32(byte *p)
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

static inline void exec_Integer_Add(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a + b);
    NEXT1;
}

static inline void exec_Integer_Subtract(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a - b);
    NEXT1;
}

static inline void exec_Integer_Multiply(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a * b);
    NEXT1;
}

static inline void exec_Integer_Divide(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a / b);
    NEXT1;
}

static inline void exec_Integer_Modulo(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a % b);
    NEXT1;
}

static inline void exec_Integer_And(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a & b);
    NEXT1;
}

static inline void exec_Integer_Or(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a | b);
    NEXT1;
}

static inline void exec_Integer_Xor(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a ^ b);
    NEXT1;
}

static inline void exec_Integer_Left_Shift(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a << b);
    NEXT1;
}

static inline void exec_Integer_Right_Shift(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a >> b);
    NEXT1;
}

static inline void exec_Integer_Greater_Than(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a > b);
    NEXT1;
}

static inline void exec_Integer_Greater_Than_Equals(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a >= b);
    NEXT1;
}

static inline void exec_Integer_Less_Than(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a < b);
    NEXT1;
}

static inline void exec_Integer_Less_Than_Equals(Malang_VM &vm)
{
    auto b = POP();
    auto a = POP();
    PUSH(a <= b);
    NEXT1;
}

static inline void exec_Integer_Negate(Malang_VM &vm)
{
    auto a = POP();
    PUSH(-a);
    NEXT1;
}

static inline void exec_Integer_Invert(Malang_VM &vm)
{
    auto a = POP();
    PUSH(~a);
    NEXT1;
}

static inline void exec_Noop(Malang_VM &vm)
{
    NEXT1;
}

static inline void exec_Literal_8(Malang_VM &vm)
{
    NEXT1;
    auto one = fetch8(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(one));
    NEXT1;
}

static inline void exec_Literal_16(Malang_VM &vm)
{
    NEXT1;
    auto two = fetch16(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(two));
    NEXT2;
}

static inline void exec_Literal_32(Malang_VM &vm)
{
    NEXT1;
    auto four = fetch32(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(four));
    NEXT4;
}

static inline void exec_Literal_int(Malang_VM &vm)
{
    NEXT1;
    auto ptr = fetch_int(vm.ip);
    vm.data_stack.push_back(ptr);
    NEXTPTR;
}

static inline void exec_Get_Type(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Branch(Malang_VM &vm)
{
    NEXT1;
    auto n = fetch_int(vm.ip);
    vm.ip += n; // XXX: n-1 to be relative to the branch instruction
}

static inline void exec_Branch_If_Zero(Malang_VM &vm)
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

static inline void exec_Leave(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Return(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Global(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Cast_To(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Add(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Subtract(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Multiply(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Divide(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Modulo(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_And(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Or(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Xor(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Left_Shift(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Right_Shift(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Greater_Than(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Greater_Than_Equals(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Less_Than(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Less_Than_Equals(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Negate(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Invert(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_Not(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Call_Method_To_String(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Global(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Local(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Field(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Global(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Local(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Field(Malang_VM &vm)
{
    NOT_IMPL;
}

static void run_code(Malang_VM &vm)
{
    auto end = &vm.code[vm.code.size()];
    while (vm.ip != end)
    {
        auto ins = static_cast<Instruction>(fetch8(vm.ip));
        switch (ins)
        {
        // We use continue instead of break so the compiler will warn us when
        // we forget to add an instruction to our dispatch and so an out-of-
        // range case will cause a trace_abort
#define ITEM(X, Y) case Instruction::X: exec_##X(vm); continue;
            #include "instruction.def"
            case Instruction::INSTRUCTION_ENUM_SIZE:
                break;
        }
        trace_abort(vm, "Unknown instruction: %x\n", static_cast<int>(ins));
    }
}
