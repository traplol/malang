#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
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


static void vprptr(const char *fmt, va_list vargs)
{
    vprintf(fmt, vargs);
}

static void prptr(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprptr(fmt, args);
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
            prptr("\n");
        }
    }
    printf("\n\nDATA:\n");
    auto n = std::min(16ul, vm.data_top);
    for (auto i = vm.data_top - n; i < vm.data_top; ++i)
    {
        auto &&e = vm.data_stack[i];
        if (e.is_pointer())
        {
            printf("-%ld: POINTER: %p\n", i, e.as_pointer());
        }
        else if (e.is_double())
        {
            printf("-%ld: DOUBLE : %lf\n", i, e.as_double());
        }
        else if (e.is_fixnum())
        {
            printf("-%ld: FIXNUM : %d\n", i, e.as_fixnum());
        }
    }
    printf("\n");
}

void trace_abort(Malang_VM &vm, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprptr(fmt, args);
    va_end(args);
    trace(vm);
    abort();
}

void Malang_VM::add_local(Malang_Value value)
{
    locals[locals_top++] = value;
}
void Malang_VM::add_global(Malang_Value value)
{
    globals[globals_top++] = value;
}
void Malang_VM::add_data(Malang_Value value)
{
    data_stack[data_top++] = value;
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

static inline Malang_Value fetch_value(Malang_VM &vm)
{
    auto p = &(vm.code[vm.ip]);
    return *reinterpret_cast<Malang_Value*>(p);
}

#define NEXT8   vm.ip += sizeof(byte)
#define NEXT16  vm.ip += sizeof(int16_t)
#define NEXT32  vm.ip += sizeof(int32_t)
#define NEXT_N(n) vm.ip += sizeof(n)

static inline void exec_Fixnum_Add(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() + b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Subtract(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() - b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Multiply(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() * b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Divide(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() / b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Modulo(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() % b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_And(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() & b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Or(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() | b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Xor(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() ^ b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Left_Shift(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() << b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Right_Shift(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() >> b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Greater_Than(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() > b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Greater_Than_Equals(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() >= b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Less_Than(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() < b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Less_Than_Equals(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() <= b.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Negate(Malang_VM &vm)
{
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = -a.as_fixnum();
    NEXT8;
}

static inline void exec_Fixnum_Invert(Malang_VM &vm)
{
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = ~a.as_fixnum();
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
    vm.data_stack[vm.data_top++] = n;
    NEXT8;
}

static inline void exec_Literal_16(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch16(vm);
    vm.data_stack[vm.data_top++] = n;
    NEXT_N(n);
}

static inline void exec_Literal_32(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.data_stack[vm.data_top++] = n;
    NEXT_N(n);
}

static inline void exec_Literal_value(Malang_VM &vm)
{
    // @Audit: something feels off about this, why would there every be a literal pointer?
    //         `Value`s might be in code by why would there be a pointer?
    NEXT8;
    auto n = fetch_value(vm);
    vm.data_stack[vm.data_top++] = n;
    NEXT_N(n);
}

static inline void exec_Get_Type(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Branch(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.ip += n; // XXX: n-1 to be relative to the branch instruction
}

static inline void exec_Branch_If_Zero(Malang_VM &vm)
{
    NEXT8;
    auto cond = vm.data_stack[--vm.data_top];
    // TODO: should probably have `true', `false', and `nil' value constants?
    if (cond.bits() == 0)
    {
        auto n = fetch32(vm);
        vm.ip += n; // XXX: n-1 to be relative to the branch instruction
    }
    else
    { // need to skip the offset
        NEXT32;
    }
}

static inline void exec_Branch_If_Not_Zero(Malang_VM &vm)
{
    NEXT8;
    auto cond = vm.data_stack[--vm.data_top];
    // TODO: should probably have `true', `false', and `nil' value constants?
    if (cond.bits() != 0)
    {
        auto n = fetch32(vm);
        vm.ip += n; // XXX: n-1 to be relative to the branch instruction
    }
    else
    { // need to skip the offset
        NEXT32;
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

static inline void exec_Load_Global(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.data_stack[vm.data_top++] = vm.globals[n];
    NEXT_N(n);
}

static inline void exec_Store_Global(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.globals[n] = vm.data_stack[--vm.data_top];
    NEXT_N(n);
}

static inline void exec_Load_Field(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Field(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Local(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch16(vm);
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    auto local = vm.locals[this_frame + n];
    vm.data_stack[vm.data_top++] = local;
    NEXT_N(n);
}

static inline void exec_Load_Local_0(Malang_VM &vm)
{
    NEXT8;
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    auto local = vm.locals[this_frame + 0];
    vm.data_stack[vm.data_top++] = local;
}

static inline void exec_Load_Local_1(Malang_VM &vm)
{
    NEXT8;
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    auto local = vm.locals[this_frame + 1];
    vm.data_stack[vm.data_top++] = local;
}

static inline void exec_Load_Local_2(Malang_VM &vm)
{
    NEXT8;
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    auto local = vm.locals[this_frame + 2];
    vm.data_stack[vm.data_top++] = local;
}

static inline void exec_Load_Local_3(Malang_VM &vm)
{
    NEXT8;
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    auto local = vm.locals[this_frame + 3];
    vm.data_stack[vm.data_top++] = local;
}

static inline void exec_Store_Local(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto n = fetch16(vm);
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    vm.locals[this_frame + n] = value;
    NEXT_N(n);
}

static inline void exec_Store_Local_0(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    vm.locals[this_frame + 0] = value;
}

static inline void exec_Store_Local_1(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    vm.locals[this_frame + 1] = value;
}

static inline void exec_Store_Local_2(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    vm.locals[this_frame + 2] = value;
}

static inline void exec_Store_Local_3(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto this_frame = vm.locals_frames[vm.locals_frames_top-1];
    vm.locals[this_frame + 3] = value;
}

static inline void exec_Load_Arg(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Arg_0(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Arg_1(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Arg_2(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Arg_3(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Arg(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Arg_0(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Arg_1(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Arg_2(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Store_Arg_3(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Alloc_Locals(Malang_VM &vm)
{
    NEXT8;
    vm.locals_frames[vm.locals_frames_top++] = vm.locals_top;
    auto n = fetch16(vm);
    vm.locals_top += n;
    NEXT_N(n);
}

static inline void exec_Free_Locals(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.locals_top -= n;
    NEXT_N(n);
}

static inline void exec_Dup_1(Malang_VM &vm)
{
    auto a = vm.data_stack[vm.data_top-1];
    vm.data_stack[vm.data_top++] = a;
    NEXT8;
}

static inline void exec_Dup_2(Malang_VM &vm)
{
    auto b = vm.data_stack[vm.data_top-1];
    auto a = vm.data_stack[vm.data_top-2];
    vm.data_stack[vm.data_top++] = b;
    vm.data_stack[vm.data_top++] = a;
    NEXT8;
}

static inline void exec_Swap_1(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = b;
    vm.data_stack[vm.data_top++] = a;
    NEXT8;
}

static inline void exec_Over_1(Malang_VM &vm)
{
    auto a = vm.data_stack[vm.data_top-2];
    vm.data_stack[vm.data_top++] = a;
    NEXT8;
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

static inline void exec_Drop_N(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch16(vm);
    vm.data_top -= n;
    NEXT_N(n);
}

static inline void exec_Fixnum_Equals(Malang_VM &vm)
{
    auto b = vm.data_stack[--vm.data_top];
    auto a = vm.data_stack[--vm.data_top];
    vm.data_stack[vm.data_top++] = a.as_fixnum() == b.as_fixnum();
    NEXT8;
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
#define ITEM(X) case Instruction::X: exec_##X(vm); continue;
            #include "instruction.def"
            case Instruction::INSTRUCTION_ENUM_SIZE:
                break;
        }
        trace_abort(vm, "Unknown instruction: %x\n", static_cast<int>(ins));
    }
}
