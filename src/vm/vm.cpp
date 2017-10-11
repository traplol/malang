#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <stdint.h>
#include "vm.hpp"
#include "instruction.hpp"

void Malang_VM::load_code(const std::vector<byte> &code)
{
    this->code.clear();
    this->code.insert(this->code.begin(), code.begin(), code.end());
}

static void run_code(Malang_VM&);
void Malang_VM::run()
{
    ip = code.data();
    locals.clear();
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

template<typename T>
static inline T PEEK(const std::vector<T> &v)
{
    return v.back();
}

template<typename T>
static inline T POP(std::vector<T> &v)
{
    T tmp = v.back();
    v.pop_back();
    return tmp;
}

template<typename T>
static inline void PUSH(std::vector<T> &v, T value)
{
    v.push_back(value);
}

#define NEXT8   vm.ip += sizeof(byte)
#define NEXT16  vm.ip += sizeof(int16_t)
#define NEXT32  vm.ip += sizeof(int32_t)
#define NEXTINT vm.ip += sizeof(intptr_t)

static inline void exec_Integer_Add(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a + b);
    NEXT8;
}

static inline void exec_Integer_Subtract(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a - b);
    NEXT8;
}

static inline void exec_Integer_Multiply(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a * b);
    NEXT8;
}

static inline void exec_Integer_Divide(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a / b);
    NEXT8;
}

static inline void exec_Integer_Modulo(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a % b);
    NEXT8;
}

static inline void exec_Integer_And(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a & b);
    NEXT8;
}

static inline void exec_Integer_Or(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a | b);
    NEXT8;
}

static inline void exec_Integer_Xor(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a ^ b);
    NEXT8;
}

static inline void exec_Integer_Left_Shift(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a << b);
    NEXT8;
}

static inline void exec_Integer_Right_Shift(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, a >> b);
    NEXT8;
}

static inline void exec_Integer_Greater_Than(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, static_cast<intptr_t>(a > b));
    NEXT8;
}

static inline void exec_Integer_Greater_Than_Equals(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, static_cast<intptr_t>(a >= b));
    NEXT8;
}

static inline void exec_Integer_Less_Than(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, static_cast<intptr_t>(a < b));
    NEXT8;
}

static inline void exec_Integer_Less_Than_Equals(Malang_VM &vm)
{
    auto b = POP(vm.data_stack);
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, static_cast<intptr_t>(a <= b));
    NEXT8;
}

static inline void exec_Integer_Negate(Malang_VM &vm)
{
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, -a);
    NEXT8;
}

static inline void exec_Integer_Invert(Malang_VM &vm)
{
    auto a = POP(vm.data_stack);
    PUSH(vm.data_stack, ~a);
    NEXT8;
}

static inline void exec_Noop(Malang_VM &vm)
{
    NEXT8;
}

static inline void exec_Literal_8(Malang_VM &vm)
{
    NEXT8;
    auto one = fetch8(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(one));
    NEXT8;
}

static inline void exec_Literal_16(Malang_VM &vm)
{
    NEXT8;
    auto two = fetch16(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(two));
    NEXT16;
}

static inline void exec_Literal_32(Malang_VM &vm)
{
    NEXT8;
    auto four = fetch32(vm.ip);
    vm.data_stack.push_back(static_cast<intptr_t>(four));
    NEXT32;
}

static inline void exec_Literal_int(Malang_VM &vm)
{
    NEXT8;
    auto ptr = fetch_int(vm.ip);
    vm.data_stack.push_back(ptr);
    NEXTINT;
}

static inline void exec_Get_Type(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Branch(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm.ip);
    vm.ip += n; // XXX: n-1 to be relative to the branch instruction
}

static inline void exec_Branch_If_Zero(Malang_VM &vm)
{
    NEXT8;
    auto cond = POP(vm.data_stack);
    if (cond == 0)
    {
        auto n = fetch_int(vm.ip);
        vm.ip += n; // XXX: n-1 to be relative to the branch instruction
    }
    else
    { // need to skip the offset
        NEXTINT;
    }
}

static inline void exec_Leave(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Return(Malang_VM &vm)
{
    vm.ip = POP(vm.return_stack);
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
    NEXT8;
    auto n = fetch_int(vm.ip);
    PUSH(vm.data_stack, vm.locals[vm.locals.size() - 1 - n]);
    vm.ip += sizeof(n);
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
    NEXT8;
    auto n = fetch_int(vm.ip);
    vm.locals[vm.locals.size() - 1 - n] = POP(vm.data_stack);
    vm.ip += sizeof(n);
}

static inline void exec_Store_Field(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Alloc_Globals(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm.ip);
    vm.globals.resize(vm.globals.size() + n);
    vm.ip += sizeof(n);
}

static inline void exec_Alloc_Locals(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm.ip);
    vm.locals.resize(vm.locals.size() + n);
    vm.ip += sizeof(n);
}

static inline void exec_Free_Globals(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm.ip);
    vm.globals.resize(vm.globals.size() - n);
    vm.ip += sizeof(n);
}

static inline void exec_Free_Locals(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm.ip);
    vm.locals.resize(vm.locals.size() - n);
    vm.ip += sizeof(n);
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
