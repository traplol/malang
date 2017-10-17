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
    locals_frames_top = 0;
    call_frames_top = 0;
    globals_top = 0;
    locals_top = 0;
    data_top = 0;

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
    print("\nCODE:\n");
    for (int i = 1; p != end; ++p, ++i)
    {
        print("%02x ", static_cast<int>(vm.code[p]));
        if (i % 40 == 0)
        {
            print("\n");
        }
    }
    print("\n\nDATA:\n");
    auto n = std::min(16ul, vm.data_top);
    for (auto i = vm.data_top - n; i < vm.data_top; ++i)
    {
        auto &&e = vm.data_stack[i];
        if (e.is_pointer())
        {
            print("-%ld: POINTER: %p\n", i, e.as_pointer());
        }
        else if (e.is_double())
        {
            print("-%ld: DOUBLE : %lf\n", i, e.as_double());
        }
        else if (e.is_fixnum())
        {
            print("-%ld: FIXNUM : %d\n", i, e.as_fixnum());
        }
    }
    print("\n");
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

inline
void Malang_VM::push_locals_frame(uintptr_t frame)
{
    locals_frames[locals_frames_top++] = frame;
}
inline
uintptr_t Malang_VM::pop_locals_frame()
{
    auto frame = locals_frames[--locals_frames_top];
    return frame;
}
inline
Malang_Value *Malang_VM::current_locals()
{
    auto frame = locals_frames[locals_frames_top-1];
    return &locals[frame];
}

inline
void Malang_VM::push_call_frame(Call_Frame frame)
{
    call_frames[call_frames_top++] = frame;
}
inline
Call_Frame Malang_VM::pop_call_frame()
{
    auto frame = call_frames[--call_frames_top];
    return frame;
}
inline
Call_Frame Malang_VM::current_call_frame()
{
    auto frame = call_frames[call_frames_top-1];
    return frame;
}
inline
Malang_Value *Malang_VM::current_args()
{
    auto frame = call_frames[call_frames_top-1];
    return &data_stack[frame.args_frame];
}

inline
void Malang_VM::push_globals(Malang_Value value)
{
    globals[globals_top++] = value;
}
inline
Malang_Value Malang_VM::pop_globals()
{
    auto global = globals[--globals_top];
    return global;
}
inline
void Malang_VM::push_locals(Malang_Value value)
{
    locals[locals_top++] = value;
}
inline
Malang_Value Malang_VM::pop_locals()
{
    auto local = locals[--locals_top];
    return local;
}
inline
void Malang_VM::push_data(Malang_Value value)
{
    data_stack[data_top++] = value;
}
inline
Malang_Value Malang_VM::pop_data()
{
    auto data = data_stack[--data_top];
    return data;
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
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() + b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Subtract(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() - b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Multiply(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() * b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Divide(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() / b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Modulo(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() % b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_And(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() & b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Or(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() | b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Xor(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() ^ b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Left_Shift(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() << b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Right_Shift(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() >> b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Greater_Than(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() > b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Greater_Than_Equals(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() >= b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Less_Than(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() < b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Less_Than_Equals(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() <= b.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Negate(Malang_VM &vm)
{
    auto a = vm.pop_data();
    vm.push_data(-a.as_fixnum());
    NEXT8;
}

static inline void exec_Fixnum_Invert(Malang_VM &vm)
{
    auto a = vm.pop_data();
    vm.push_data(~a.as_fixnum());
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
    vm.push_data(n);
    NEXT8;
}

static inline void exec_Literal_16(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch16(vm);
    vm.push_data(n);
    NEXT_N(n);
}

static inline void exec_Literal_32(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.push_data(n);
    NEXT_N(n);
}

static inline void exec_Literal_value(Malang_VM &vm)
{
    // @Audit: something feels off about this, why would there every be a literal pointer?
    //         `Value`s might be in code by why would there be a pointer?
    NEXT8;
    auto n = fetch_value(vm);
    vm.push_data(n);
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
    auto cond = vm.pop_data();
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
    auto cond = vm.pop_data();
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
    auto frame = vm.pop_call_frame();
    vm.ip = frame.return_ip;
}

static inline void exec_Call(Malang_VM &vm)
{
    auto new_ip = vm.pop_data().as_fixnum();
    vm.push_call_frame({vm.ip+1, vm.data_top-1});
    vm.ip = new_ip;
}

static inline void exec_Call_Virtual(Malang_VM &vm)
{
    NOT_IMPL;
}

static inline void exec_Load_Global(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.push_data(vm.globals[n]);
    NEXT_N(n);
}

static inline void exec_Store_Global(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch32(vm);
    vm.globals[n] = vm.pop_data();
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
    auto locals = vm.current_locals();
    auto local = locals[n];
    vm.push_data(local);
    NEXT_N(n);
}

static inline void exec_Load_Local_0(Malang_VM &vm)
{
    auto locals = vm.current_locals();
    auto local = locals[0];
    vm.push_data(local);
    NEXT8;
}

static inline void exec_Load_Local_1(Malang_VM &vm)
{
    auto locals = vm.current_locals();
    auto local = locals[1];
    vm.push_data(local);
    NEXT8;
}

static inline void exec_Load_Local_2(Malang_VM &vm)
{
    auto locals = vm.current_locals();
    auto local = locals[2];
    vm.push_data(local);
    NEXT8;
}

static inline void exec_Load_Local_3(Malang_VM &vm)
{
    auto locals = vm.current_locals();
    auto local = locals[3];
    vm.push_data(local);
    NEXT8;
}

static inline void exec_Store_Local(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto n = fetch16(vm);
    auto locals = vm.current_locals();
    locals[n] = value;
    NEXT_N(n);
}

static inline void exec_Store_Local_0(Malang_VM &vm)
{
    auto value = vm.pop_data();
    auto locals = vm.current_locals();
    locals[0] = value;
    NEXT8;
}

static inline void exec_Store_Local_1(Malang_VM &vm)
{
    auto value = vm.pop_data();
    auto locals = vm.current_locals();
    locals[1] = value;
    NEXT8;
}

static inline void exec_Store_Local_2(Malang_VM &vm)
{
    auto value = vm.pop_data();
    auto locals = vm.current_locals();
    locals[2] = value;
    NEXT8;
}

static inline void exec_Store_Local_3(Malang_VM &vm)
{
    auto value = vm.pop_data();
    auto locals = vm.current_locals();
    locals[3] = value;
    NEXT8;
}

static inline void exec_Load_Arg(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch16(vm);
    auto args = vm.current_args();
    auto arg = args[-n];
    vm.push_data(arg);
    NEXT_N(n);
}

static inline void exec_Load_Arg_0(Malang_VM &vm)
{
    auto args = vm.current_args();
    auto arg = args[-0];
    vm.push_data(arg);
    NEXT8;
}

static inline void exec_Load_Arg_1(Malang_VM &vm)
{
    auto args = vm.current_args();
    auto arg = args[-1];
    vm.push_data(arg);
    NEXT8;
}

static inline void exec_Load_Arg_2(Malang_VM &vm)
{
    auto args = vm.current_args();
    auto arg = args[-2];
    vm.push_data(arg);
    NEXT8;
}

static inline void exec_Load_Arg_3(Malang_VM &vm)
{
    auto args = vm.current_args();
    auto arg = args[-3];
    vm.push_data(arg);
    NEXT8;
}

static inline void exec_Store_Arg(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto n = fetch16(vm);
    auto args = vm.current_args();
    args[-n] = value;
    NEXT_N(n);
}

static inline void exec_Store_Arg_0(Malang_VM &vm)
{
    auto value = vm.pop_data();
    auto args = vm.current_args();
    args[-0] = value;
    NEXT8;
}

static inline void exec_Store_Arg_1(Malang_VM &vm)
{
    auto value = vm.pop_data();
    auto args = vm.current_args();
    args[-1] = value;
    NEXT8;
}

static inline void exec_Store_Arg_2(Malang_VM &vm)
{
    auto value = vm.pop_data();
    auto args = vm.current_args();
    args[-2] = value;
    NEXT8;
}

static inline void exec_Store_Arg_3(Malang_VM &vm)
{
    auto value = vm.pop_data();
    auto args = vm.current_args();
    args[-3] = value;
    NEXT8;
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
    auto a = vm.pop_data();
    vm.push_data(a);
    vm.push_data(a);
    NEXT8;
}

static inline void exec_Dup_2(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a);
    vm.push_data(b);
    vm.push_data(a);
    vm.push_data(b);
    NEXT8;
}

static inline void exec_Swap_1(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(b);
    vm.push_data(a);
    NEXT8;
}

static inline void exec_Over_1(Malang_VM &vm)
{
    auto a = vm.data_stack[vm.data_top-2];
    vm.push_data(a);
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
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() == b.as_fixnum());
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
