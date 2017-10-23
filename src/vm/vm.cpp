#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <sstream>
#include "vm.hpp"
#include "instruction.hpp"
#include "runtime/gc.hpp"

Malang_VM::Malang_VM(const std::vector<Native_Code> &primitives, size_t gc_run_interval)
    : primitives(std::move(primitives))
{
    gc = new Malang_GC{this, gc_run_interval};
}

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

static inline
std::string to_string(const Malang_Value &value)
{
    std::stringstream ss;
    if (value.is_fixnum())
    {
        ss << value.as_fixnum();
    }
    else if (value.is_double())
    {
        ss << value.as_double();
    }
    else if (value.is_object())
    {
        auto obj = value.as_object();
        ss << "Object(" << obj->type->name() << "#" << obj << ")";
    }
    else if (value.is_pointer())
    {
        ss << "Pointer(" << value.as_pointer<void>() << ")";
    }
    else
    {
        ss << "?(" << std::hex << value.bits() << ")";
    }
    return ss.str();
}

void Malang_VM::stack_trace(uintptr_t n) const
{
    print("\nDATA STACK:\n");
    n = std::min(n, data_top);
    for (auto i = data_top - n; i < data_top; ++i)
    {
        auto &&e = data_stack[i];
        if (n-i-1 == 0)
        {
            print("-%ld: %s <-- TOP\n", n-i-1, to_string(e).c_str());
        }
        else
        {
            print("-%ld: %s\n", n-i-1, to_string(e).c_str());
        }
    }
    print("\n");
}

void Malang_VM::trace() const
{
    auto x = std::min(64ul, ip);
    auto y = std::min(64ul, code.size() - ip);

    auto start = ip - x;
    auto end = ip + y;
    auto p = start;
    print("\nCODE:\n");
    for (int i = 1; p != end; ++p, ++i)
    {
        print("%02x ", static_cast<int>(code[p]));
        if (i % 40 == 0)
        {
            print("\n");
        }
    }
    print("\n");
    stack_trace();
}

void Malang_VM::trace_abort(const char *fmt, ...) const
{
    va_list args;
    va_start(args, fmt);
    vprint(fmt, args);
    va_end(args);
    trace();
    abort();
}

void Malang_VM::dump_code(uintptr_t ip, size_t n, int width) const
{
    auto mem = code.data() + ip;
    n = std::min(ip+n, code.size());
    for (size_t i = 0; i < n; ++i)
    {
        if (i % width == 0)
        {
            printf("%08lx  ", i);
        }
        printf("%02x ", mem[i]);
        if ((i+1) % width == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
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
Malang_Value Malang_VM::get_local(intptr_t n)
{
    return current_locals()[0+n];
}
inline
void Malang_VM::set_local(intptr_t n, Malang_Value value)
{
    current_locals()[0+n] = value;
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
Malang_Value Malang_VM::get_arg(intptr_t n)
{
    return current_args()[0-n];
}
inline
void Malang_VM::set_arg(intptr_t n, Malang_Value value)
{
    current_args()[0-n] = value;
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

#define NOT_IMPL {vm.trace_abort("%s:%d `%s()` not implemented\n", __FILE__, __LINE__, __FUNCTION__);}

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
    vm.ip += n-1; // @XXX: n-1 to be relative to the branch instruction
}

static inline void exec_Branch_If_Zero(Malang_VM &vm)
{
    NEXT8;
    auto cond = vm.pop_data();
    // @TODO: should probably have `true', `false', and `nil' value constants?
    if (cond.bits() == 0)
    {
        auto n = fetch32(vm);
        vm.ip += n-1; // @XXX: n-1 to be relative to the branch instruction
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
    // @TODO: should probably have `true', `false', and `nil' value constants?
    if (cond.bits() != 0)
    {
        auto n = fetch32(vm);
        vm.ip += n-1; // @XXX: n-1 to be relative to the branch instruction
    }
    else
    { // need to skip the offset
        NEXT32;
    }
}

static inline void exec_Return(Malang_VM &vm)
{
    auto frame = vm.pop_call_frame();
    vm.ip = frame.return_ip;
}

static inline void exec_Call(Malang_VM &vm)
{
    NEXT8;
    auto new_ip = fetch32(vm);
    vm.push_call_frame({vm.ip + sizeof(new_ip), vm.data_top-1});
    vm.ip = new_ip;
}

static inline void exec_Call_Primitive(Malang_VM &vm)
{
    NEXT8;
    auto prim_fn_idx = fetch32(vm);
    vm.push_call_frame({vm.ip + sizeof(prim_fn_idx), vm.data_top-1});
    vm.primitives[prim_fn_idx](vm);
    exec_Return(vm);
}

static inline void exec_Call_Dyn(Malang_VM &vm)
{
    auto new_ip = vm.pop_data().as_fixnum();
    vm.push_call_frame({vm.ip+1, vm.data_top-1});
    vm.ip = new_ip;
}

static inline void exec_Call_Primitive_Dyn(Malang_VM &vm)
{
    auto prim_fn_idx = vm.pop_data().as_fixnum();
    vm.push_call_frame({vm.ip+1, vm.data_top-1});
    vm.primitives[prim_fn_idx](vm);
    exec_Return(vm);
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
    auto local = vm.get_local(n);
    vm.push_data(local);
    NEXT_N(n);
}

static inline void exec_Load_Local_0(Malang_VM &vm)
{
    auto local = vm.get_local(0);
    vm.push_data(local);
    NEXT8;
}

static inline void exec_Load_Local_1(Malang_VM &vm)
{
    auto local = vm.get_local(1);
    vm.push_data(local);
    NEXT8;
}

static inline void exec_Load_Local_2(Malang_VM &vm)
{
    auto local = vm.get_local(2);
    vm.push_data(local);
    NEXT8;
}

static inline void exec_Load_Local_3(Malang_VM &vm)
{
    auto local = vm.get_local(3);
    vm.push_data(local);
    NEXT8;
}

static inline void exec_Store_Local(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto n = fetch16(vm);
    vm.set_local(n, value);
    NEXT_N(n);
}

static inline void exec_Store_Local_0(Malang_VM &vm)
{
    auto value = vm.pop_data();
    vm.set_local(0, value);
    NEXT8;
}

static inline void exec_Store_Local_1(Malang_VM &vm)
{
    auto value = vm.pop_data();
    vm.set_local(1, value);
    NEXT8;
}

static inline void exec_Store_Local_2(Malang_VM &vm)
{
    auto value = vm.pop_data();
    vm.set_local(2, value);
    NEXT8;
}

static inline void exec_Store_Local_3(Malang_VM &vm)
{
    auto value = vm.pop_data();
    vm.set_local(3, value);
    NEXT8;
}

static inline void exec_Load_Arg(Malang_VM &vm)
{
    NEXT8;
    auto n = fetch16(vm);
    auto arg = vm.get_arg(n);
    vm.push_data(arg);
    NEXT_N(n);
}

static inline void exec_Load_Arg_0(Malang_VM &vm)
{
    auto arg = vm.get_arg(0);
    vm.push_data(arg);
    NEXT8;
}

static inline void exec_Load_Arg_1(Malang_VM &vm)
{
    auto arg = vm.get_arg(1);
    vm.push_data(arg);
    NEXT8;
}

static inline void exec_Load_Arg_2(Malang_VM &vm)
{
    auto arg = vm.get_arg(2);
    vm.push_data(arg);
    NEXT8;
}

static inline void exec_Load_Arg_3(Malang_VM &vm)
{
    auto arg = vm.get_arg(3);
    vm.push_data(arg);
    NEXT8;
}

static inline void exec_Store_Arg(Malang_VM &vm)
{
    NEXT8;
    auto value = vm.data_stack[--vm.data_top];
    auto n = fetch16(vm);
    vm.set_arg(n, value);
    NEXT_N(n);
}

static inline void exec_Store_Arg_0(Malang_VM &vm)
{
    auto value = vm.pop_data();
    vm.set_arg(0, value);
    NEXT8;
}

static inline void exec_Store_Arg_1(Malang_VM &vm)
{
    auto value = vm.pop_data();
    vm.set_arg(1, value);
    NEXT8;
}

static inline void exec_Store_Arg_2(Malang_VM &vm)
{
    auto value = vm.pop_data();
    vm.set_arg(2, value);
    NEXT8;
}

static inline void exec_Store_Arg_3(Malang_VM &vm)
{
    auto value = vm.pop_data();
    vm.set_arg(3, value);
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
    auto n = fetch16(vm);
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

static inline void exec_Fixnum_Not_Equals(Malang_VM &vm)
{
    auto b = vm.pop_data();
    auto a = vm.pop_data();
    vm.push_data(a.as_fixnum() != b.as_fixnum());
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
        vm.trace_abort("Unknown instruction: %x\n", static_cast<int>(ins));
    }
}
