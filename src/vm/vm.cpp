#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <stdint.h>
#include <string.h>
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
    ip = 0;
    globals_top = 0;
    locals_top = 0;
    data_top = 0;
    return_top = 0;
    locals_frames_top = 0;

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
    auto x = std::min(64ul, vm.ip);
    auto y = std::min(64ul, vm.code.size() - vm.ip);

    auto start = vm.ip - x;
    auto end = vm.ip + y;
    auto p = start;
    printf("\nCODE:\n");
    for (int i = 1; p != end; ++p, ++i)
    {
        printf("%02x ", static_cast<int>(vm.code[p]));
        if (i % 40 == 0)
        {
            print("\n");
        }
    }
    printf("\n\nDATA:\n");
    auto n = std::min(16ul, vm.data_top);
    for (auto i = vm.data_top - n; i < vm.data_top; ++i)
    {
        printf("-%ld: %ld\n", i, vm.data_stack[i]);
    }
    printf("\n");
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

static inline byte fetch8(Malang_VM &vm)
{
    auto p = &(vm.code[vm.ip]);
    return *p;
}

static inline int16_t fetch16(Malang_VM &vm)
{
    auto p = &(vm.code[vm.ip]);
    return *reinterpret_cast<int16_t*>(p);
}

static inline int32_t fetch32(Malang_VM &vm)
{
    auto p = &(vm.code[vm.ip]);
    return *reinterpret_cast<int32_t*>(p);
}

static inline intptr_t fetch_int(Malang_VM &vm)
{
    auto p = &(vm.code[vm.ip]);
    return *reinterpret_cast<intptr_t*>(p);
}

#define NEXT8   vm.ip += sizeof(byte)
#define NEXT16  vm.ip += sizeof(int16_t)
#define NEXT32  vm.ip += sizeof(int32_t)
#define NEXTINT vm.ip += sizeof(intptr_t)

static inline void exec_Integer_Add(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a + b;
    NEXT8;
}

static inline void exec_Integer_Subtract(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a - b;
    NEXT8;
}

static inline void exec_Integer_Multiply(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a * b;
    NEXT8;
}

static inline void exec_Integer_Divide(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a / b;
    NEXT8;
}

static inline void exec_Integer_Modulo(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a % b;
    NEXT8;
}

static inline void exec_Integer_And(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a & b;
    NEXT8;
}

static inline void exec_Integer_Or(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a | b;
    NEXT8;
}

static inline void exec_Integer_Xor(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a ^ b;
    NEXT8;
}

static inline void exec_Integer_Left_Shift(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a << b;
    NEXT8;
}

static inline void exec_Integer_Right_Shift(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a >> b;
    NEXT8;
}

static inline void exec_Integer_Greater_Than(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a > b;
    NEXT8;
}

static inline void exec_Integer_Greater_Than_Equals(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a >= b;
    NEXT8;
}

static inline void exec_Integer_Less_Than(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a < b;
    NEXT8;
}

static inline void exec_Integer_Less_Than_Equals(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a <= b;
    NEXT8;
}

static inline void exec_Integer_Negate(Malang_VM &vm)
{
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = -a;
    NEXT8;
}

static inline void exec_Integer_Invert(Malang_VM &vm)
{
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = ~a;
    NEXT8;
}

static inline void exec_Noop(Malang_VM &vm)
{
    NEXT8;
}

static inline void exec_Literal_8(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch8(vm);
    vm.data_stack[vm.data_top++] = static_cast<intptr_t>(n);
    NEXT8;
}

static inline void exec_Literal_16(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch16(vm);
    vm.data_stack[vm.data_top++] = static_cast<intptr_t>(n);
    NEXT16;
}

static inline void exec_Literal_32(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.data_stack[vm.data_top++] = static_cast<intptr_t>(n);
    NEXT32;
}

static inline void exec_Literal_int(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm);
    vm.data_stack[vm.data_top++] = n;
    NEXTINT;
}

static inline void exec_Get_Type(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Branch(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm);
    vm.ip += n; // XXX: n-1 to be relative to the branch instruction
}

static inline void exec_Branch_If_Zero(Malang_VM &vm)
{
    NEXT8;
    auto cond = vm.data_stack[--vm.data_top];
    if (cond == 0)
    {
        auto n = fetch_int(vm);
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
    vm.ip = vm.return_stack[--vm.return_top];
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
    auto n = fetch_int(vm);
    vm.data_stack[vm.data_top++] = vm.globals[n];
}

static inline void exec_Load_Local(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm);
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    auto local = vm.locals[this_frame + n];
    vm.data_stack[vm.data_top++] = local;
    NEXTINT;
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
    auto value = vm.data_stack[--vm.data_top];
    auto n = fetch_int(vm);
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    vm.locals[this_frame + n] = value;
    vm.ip += sizeof(n);
}

static inline void exec_Store_Field(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Alloc_Globals(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto n = fetch_int(vm);
    vm.globals[n] = value;
    NEXTINT;
}

static inline void exec_Alloc_Locals(Malang_VM &vm)
{
    NEXT8;
    vm.locals_frames[vm.locals_frames_top++] = vm.locals_top;
    auto n = fetch_int(vm);
    vm.locals_top += n;
    NEXTINT;
}

static inline void exec_Free_Globals(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Free_Locals(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm);
    vm.locals_top -= n;
    NEXTINT;
}

static inline void exec_Drop_1(Malang_VM &vm)
{
    vm.data_top -= 1;
    NEXT8;
}

static inline void exec_Drop_2(Malang_VM &vm)
{
    vm.data_top -= 2;
    NEXT8;
}

static inline void exec_Drop_3(Malang_VM &vm)
{
    vm.data_top -= 3;
    NEXT8;
}

static inline void exec_Drop_4(Malang_VM &vm)
{
    vm.data_top -= 4;
    NEXT8;
}

static inline void exec_Drop_5(Malang_VM &vm)
{
    vm.data_top -= 5;
    NEXT8;
}

static inline void exec_Drop_N(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch_int(vm);
    vm.data_top -= n;
    NEXTINT;
}


static void run_code(Malang_VM &vm)
{
    while (vm.ip < vm.code.size())
    {
        auto ins = static_cast<Instruction>(fetch8(vm));
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
