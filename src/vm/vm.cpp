#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <iostream>
#include "vm.hpp"
#include "instruction.hpp"
#include "runtime/gc.hpp"
#include "runtime.hpp"
#include "../codegen/disassm.hpp"


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

Malang_VM::~Malang_VM()
{
    locals_frames_top = 0;
    call_frames_top = 0;
    globals_top = 0;
    locals_top = 0;
    data_top = 0;
    delete gc;
}

Malang_VM::Malang_VM(Args *args,
                     Type_Map *types,
                     const std::vector<Native_Code> &natives,
                     const std::vector<String_Constant> &string_constants,
                     size_t gc_run_interval, size_t max_num_objects)
    : natives(natives)
    , string_constants(string_constants)
    , types(types)
    , breaking(false)
{
    gc = new Malang_GC{args, this, types, gc_run_interval, max_num_objects};
    auto str_ty = types->get_string();
    for (auto &&sc : string_constants)
    {
        auto obj = gc->allocate_unmanaged_object(str_ty->type_token());
        if (!obj)
        {
            panic("GC couldn't allocate unmanaged string constant.\n");
        }
        Malang_Runtime::string_construct_intern(obj, sc);
        string_constants_objects.push_back(obj);
    }
}

void Malang_VM::load_code(const std::vector<byte> &code)
{
    this->code.clear();
    this->code.insert(this->code.begin(), code.begin(), code.end());
}

static void run_code(Malang_VM&);
void Malang_VM::run()
{
    locals_frames_top = 0;
    call_frames_top = 0;
    globals_top = 0;
    locals_top = 0;
    data_top = 0;

    run_code(*this);
}

void Malang_VM::panic(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    print("runtime panic trigger!\n    ");
    vprint(fmt, args);
    va_end(args);
    print("\n");
    stack_trace();
#if DEBUG_MODE
    throw "panic";
#else
    abort();
#endif
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
        if (obj->object_tag == Array) {
            auto arr = reinterpret_cast<Malang_Array*>(obj);
            ss << "<[" << arr->size << "]" << obj->type->name() << "#" << obj << ">";
        }
        else if (obj->object_tag == Buffer) {
            ss << "<" << obj->type->name() << "#" << obj << ">";
        }
        else if (obj->type->name() == "string") {
            auto str = reinterpret_cast<Malang_Object_Body*>(obj);
            ss << "<" << obj->type->name() << "#" << obj << "> \"";
            std::string s((char*)(str->fields[1].as_pointer()), str->fields[0].as_fixnum());
            auto sub = s.substr(0, 100);
            ss << sub << '"';
            if (s.size() > 100)
            {
                ss << "...(+" << s.size() - sub.size() << " more)";
            }
        }
        else {
            ss << "<" << obj->type->name() << "#" << obj << ">";
        }
    }
    else
    {
        ss << "?(" << std::hex << value.bits() << ")";
    }
    return ss.str();
}

void Malang_VM::stack_trace() const
{
    print("\nDATA STACK:\n");
    if ((int)data_top < 0)
    {
        print("data top underflowed! (%x)\n", data_top);
    }
    else
    {
        for (auto i = data_top; i != 0; --i)
        {
            auto &&e = data_stack[i-1];
            if (i == data_top)
            {
                print("%ld: %s <-- TOP\n", data_top-i, to_string(e).c_str());
            }
            else
            {
                print("-%ld: %s\n", data_top-i, to_string(e).c_str());
            }
        }
    }
    print("\nLOCALS:\n");


    auto tmp_locals_frames_top = locals_frames_top;
    uintptr_t this_frame_ends_at = 0;
    for (auto i = locals_top-1; i != static_cast<uintptr_t>(-1); --i)
    {
        if (tmp_locals_frames_top > 0)
        {
            this_frame_ends_at = locals_frames[tmp_locals_frames_top-1];
        }
        auto local = locals[i];
        print("%d: %s\n", i-this_frame_ends_at, to_string(local).c_str());
        if (i == this_frame_ends_at)
        {
            tmp_locals_frames_top--;
            print("~~~~~~~~~\n");
        }
    }


    print("\nGLOBALS:\n");
    for (size_t i = 0; i <= globals_top; ++i)
    {
        auto &&e = globals[i];
        print("%ld: %s\n", i, to_string(e).c_str());
    }
    print("\n");
}

void Malang_VM::trace(uintptr_t ip) const
{
    auto x = std::min(static_cast<uintptr_t>(64ul), ip);
    auto y = std::min(static_cast<uintptr_t>(64ul), code.size() - ip);

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

void Malang_VM::trace_abort(uintptr_t ip, const char *fmt, ...) const
{
    va_list args;
    va_start(args, fmt);
    vprint(fmt, args);
    va_end(args);
    trace(ip);
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
            print("%08lx  ", i);
        }
        print("%02x ", mem[i]);
        if ((i+1) % width == 0)
        {
            print("\n");
        }
    }
    print("\n");
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


#define NOT_IMPL {vm.trace_abort(ip-vm.code.data(), "%s:%d `%s()` not implemented\n", __FILE__, __LINE__, __FUNCTION__);}

static inline
byte fetch8(byte *p)
{
    return *p;
}

static inline
int16_t fetch16(byte *p)
{
    return *reinterpret_cast<int16_t*>(p);
}

static inline
int32_t fetch32(byte *p)
{
    return *reinterpret_cast<int32_t*>(p);
}

static inline
Malang_Value fetch_value(byte *p)
{
    return *reinterpret_cast<Malang_Value*>(p);
}

static
void dbg_dis(Malang_VM &vm, byte *ip, int n)
{
    std::string str;
    for (int i = 0; i < n; ++i)
    {
        auto offset = ip - vm.code.data();
        ip = Disassembler::dis1(ip, offset, str);
        print("%s\n", str.c_str());
    }
}

static
void debugger(Malang_VM &vm, byte *ip)
{
    static std::string last_cmd;
    static int step_n;
    static bool done = false, waiting_for_return = false;
    if (done) { return; }
    if (waiting_for_return)
    {
        dbg_dis(vm, ip, 1);
        auto ins = static_cast<Instruction>(fetch8(ip));
        if (ins == Instruction::Return || ins == Instruction::Return_Fast)
        {
            waiting_for_return = false;
        }
        else
        {
            return;
        }
    }
    dbg_dis(vm, ip, 1);
    if (step_n > 0)
    {
        --step_n;
        return;
    }
    std::string line, cmd;
    while (true)
    {
        print(">>> ");
        if (!std::getline(std::cin, line))
        {
            done = true;
            break;
        }
        int arg0 = 0, arg1 = 0, arg2 = 0;
        if (std::all_of(line.begin(), line.end(), isspace))
        {
            cmd = last_cmd;
        }
        else
        {
            cmd = line;
        }
        last_cmd = cmd;
        if (!cmd.empty())
        {
            char cmd_buf[128]{0};
            sscanf(cmd.data(), "%s %x %x %x", cmd_buf, &arg0, &arg1, &arg2);
            std::string cmd_str(cmd_buf);
            if (cmd_str == "help")
            {
                print("Command arguments are in hex\n");
                print("help               display this message\n");
                print("s(tep) n=1         step through n instructions\n");
                print("run                runs until next breakpoint\n");
                print("ret(urn)           runs until next return\n");
                print("st(race)           prints a stack trace\n");
                print("dis n=1 a=HERE     disassembles n instructions at a(ddress)\n");
                print("local n=0          prints local variable n\n");
                print("stack n=0          prints stack variable n\n");
                print("to(p)              prints top of stack\n");
                print("se(cond)           prints second of stack\n");
                print("th(ird)            prints third of stack\n");
                print("fo(urth)           prints fourth of stack\n");
            }
            else if (cmd_str == "s" || cmd_str == "step")
            {
                step_n = arg0-1;
                return;
            }
            else if (cmd_str == "run")
            {
                vm.breaking = false;
                return;
            }
            else if (cmd_str == "ret" || cmd_str == "return")
            {
                waiting_for_return = true;
                return;
            }
            else if (cmd_str == "st" || cmd_str == "strace")
            {
                vm.stack_trace();
            }
            else if (cmd_str == "dis")
            {
                auto n = arg0 > 0 ? arg0 : 1;
                if (arg1)
                {
                    dbg_dis(vm, vm.code.data() + arg1, n);
                }
                else
                {
                    dbg_dis(vm, ip, n);
                }
            }
            else if (cmd_str == "local")
            {
                print("LOCAL %x: %s\n", arg0, to_string(vm.get_local(arg0)).c_str());
            }
            else if (cmd_str == "stack")
            {
                print("STACK %x: %s\n", arg0, to_string(vm.peek_data(arg0)).c_str());
            }
            else if (cmd_str == "to" || cmd_str == "top")
            {
                print("STACK 0: %s\n", to_string(vm.peek_data(0)).c_str());
            }
            else if (cmd_str == "se" || cmd_str == "second")
            {
                print("STACK 1: %s\n", to_string(vm.peek_data(1)).c_str());
            }
            else if (cmd_str == "th" || cmd_str == "third")
            {
                print("STACK 2: %s\n", to_string(vm.peek_data(2)).c_str());
            }
            else if (cmd_str == "fo" || cmd_str == "fourth")
            {
                print("STACK 3: %s\n", to_string(vm.peek_data(3)).c_str());
            }
        }
    }
}

static
void run_code(Malang_VM &vm)
{
#ifndef USE_COMPUTED_GOTO
#define USE_COMPUTED_GOTO 0
#endif

#if USE_COMPUTED_GOTO

#define ITEM(X) &&computed_##X,
    void *computed_gotos[] =
        {
            &&computed_Halt,
#include "instruction.def"
        };

#define EXEC                               \
    goto *computed_gotos[fetch8(ip)];
  
#define DISPATCH(X) computed_##X:
  
#define HALT                                    \
    computed_Halt:                              \
        { return; }

#define DISPATCH_NEXT \
        goto *computed_gotos[fetch8(ip)]

#define VM_INIT // empty

#else

#define DISPATCH(X) case Instruction::X: 

#define DISPATCH_NEXT continue

#define EXEC                                       \
    auto ins = static_cast<Instruction>(fetch8(ip));    \
    if (vm.breaking) { debugger(vm, ip); }                \
    switch (ins)
  
#define HALT                                            \
    default: vm.trace_abort(ip - first_ip, "Unknown instruction");   \
    case Instruction::Halt: return;                     \

#define VM_INIT for (;;)
#endif

    if (vm.code.empty())
        return;
    auto ip = vm.code.data();
    auto first_ip = ip;
    auto fast_locals = vm.locals;
    auto prev_ins_ip = ip;
    #if DEBUG_MODE
    try
    {
    #endif

    VM_INIT
    {
        prev_ins_ip = ip;
        EXEC
        {
            HALT;
            DISPATCH(Fixnum_Add)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a+b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Subtract)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a-b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Multiply)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a*b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Divide)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a/b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Modulo)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a%b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_And)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a&b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Or)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a|b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Xor)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a^b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Left_Shift)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a<<b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Right_Shift)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a>>b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Equals)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a==b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Not_Equals)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a!=b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Greater_Than)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a>b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Greater_Than_Equals)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a>=b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Less_Than)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a<b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Less_Than_Equals)
            {
                ip++;
                auto b = vm.pop_data().as_fixnum();
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(a<=b);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Negate)
            {
                ip++;
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(-a);
                DISPATCH_NEXT;
            }
            DISPATCH(Fixnum_Invert)
            {
                ip++;
                auto a = vm.pop_data().as_fixnum();
                vm.push_data(~a);
                DISPATCH_NEXT;
            }
            DISPATCH(Noop)
            {
                ip++;
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_8)
            {
                ip++;
                auto n = fetch8(ip);
                ip += sizeof(n);
                vm.push_data(static_cast<Fixnum>(n));
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_16)
            {
                ip++;
                auto n = fetch16(ip);
                ip += sizeof(n);
                vm.push_data(static_cast<Fixnum>(n));
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_32)
            {
                ip++;
                auto n = fetch32(ip);
                ip += sizeof(n);
                vm.push_data(static_cast<Fixnum>(n));
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_value)
            {
                ip++;
                auto n = fetch_value(ip);
                ip += sizeof(n);
                vm.push_data(n);
                DISPATCH_NEXT;
            }
            DISPATCH(Get_Type)
            {
                NOT_IMPL;
            }
            DISPATCH(Branch)
            {
                ip++;
                ip += fetch32(ip) - 1;
                DISPATCH_NEXT;
            }
            DISPATCH(Pop_Branch_If_False)
            {
                ip++;
                if (vm.pop_data().as_fixnum() == 0)
                {
                    ip += fetch32(ip) - 1;
                }
                else
                {
                    ip += 4;
                }
                DISPATCH_NEXT;
            }
            DISPATCH(Pop_Branch_If_True)
            {
                ip++;
                if (vm.pop_data().as_fixnum() != 0)
                {
                    ip += fetch32(ip) - 1;
                }
                else
                {
                    ip += 4;
                }
                DISPATCH_NEXT;
            }
            DISPATCH(Branch_If_False_Or_Pop)
            {
                ip++;
                if (vm.peek_data().as_fixnum() == 0)
                {
                    ip += fetch32(ip) - 1;
                }
                else
                {
                    vm.pop_data();
                    ip += 4;
                }
                DISPATCH_NEXT;
            }
            DISPATCH(Branch_If_True_Or_Pop)
            {
                ip++;
                if (vm.peek_data().as_fixnum() != 0)
                {
                    ip += fetch32(ip) - 1;
                }
                else
                {
                    vm.pop_data();
                    ip += 4;
                }
                DISPATCH_NEXT;
            }
            DISPATCH(Return)
            {
                vm.locals_top = vm.pop_locals_frame();
                fast_locals = vm.current_locals();
                ip = vm.pop_call_frame();
                DISPATCH_NEXT;
            }
            DISPATCH(Return_Fast)
            {
                ip = vm.pop_call_frame();
                DISPATCH_NEXT;
            }
            DISPATCH(Call)
            {
                ip++;
                auto idx = fetch32(ip);
                vm.push_call_frame(ip + sizeof(idx));
                ip = first_ip + idx;
                DISPATCH_NEXT;
            }
            DISPATCH(Call_Native)
            {
                ip++;
                auto idx = fetch32(ip);
                ip += sizeof(idx);
                vm.natives[idx](vm);
                DISPATCH_NEXT;
            }
            DISPATCH(Call_Dyn)
            {
                auto new_ip = vm.pop_data().as_fixnum();
                vm.push_call_frame(ip + 1);
                ip = first_ip + new_ip;
                DISPATCH_NEXT;
            }
            DISPATCH(Call_Native_Dyn)
            {
                ip++;
                auto idx = vm.pop_data().as_fixnum();
                vm.natives[idx](vm);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Global)
            {
                ip++;
                auto n = fetch32(ip);
                ip += sizeof(n);
                auto v = vm.globals[n];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Global)
            {
                ip++;
                auto n = fetch32(ip);
                if (n > (int)vm.globals_top)
                {
                    vm.globals_top = n;
                }
                ip += sizeof(n);
                auto v = vm.pop_data();
                vm.globals[n] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Field)
            {
                ip++;
                auto idx = fetch16(ip);
                ip += sizeof(idx);
                auto obj = reinterpret_cast<Malang_Object_Body*>(vm.pop_data().as_object());
                vm.push_data(obj->fields[idx]);
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Field)
            {
                ip++;
                auto idx = fetch16(ip);
                ip += sizeof(idx);
                auto obj = reinterpret_cast<Malang_Object_Body*>(vm.pop_data().as_object());
                auto value = vm.pop_data();
                obj->fields[idx] = value;
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local)
            {
                ip++;
                auto n = fetch16(ip);
                ip += sizeof(n);
                auto v = fast_locals[n];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_0)
            {
                ip++;
                auto v = fast_locals[0];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_1)
            {
                ip++;
                auto v = fast_locals[1];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_2)
            {
                ip++;
                auto v = fast_locals[2];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_3)
            {
                ip++;
                auto v = fast_locals[3];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_4)
            {
                ip++;
                auto v = fast_locals[4];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_5)
            {
                ip++;
                auto v = fast_locals[5];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_6)
            {
                ip++;
                auto v = fast_locals[6];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_7)
            {
                ip++;
                auto v = fast_locals[7];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_8)
            {
                ip++;
                auto v = fast_locals[8];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_Local_9)
            {
                ip++;
                auto v = fast_locals[9];
                vm.push_data(v);
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local)
            {
                ip++;
                auto n = fetch16(ip);
                ip += sizeof(n);
                auto v = vm.pop_data();
                fast_locals[n] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_0)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[0] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_1)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[1] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_2)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[2] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_3)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[3] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_4)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[4] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_5)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[5] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_6)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[6] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_7)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[7] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_8)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[8] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Store_Local_9)
            {
                ip++;
                auto v = vm.pop_data();
                fast_locals[9] = v;
                DISPATCH_NEXT;
            }
            DISPATCH(Alloc_Locals)
            {
                ip++;
                vm.push_locals_frame(vm.locals_top);
                auto n = fetch16(ip);
                ip += sizeof(n);
                vm.locals_top += n;
                fast_locals = vm.current_locals();
                DISPATCH_NEXT;
            }
            DISPATCH(Dup_1)
            {
                ip++;
                auto a = vm.pop_data();
                vm.push_data(a);
                vm.push_data(a);
                DISPATCH_NEXT;
            }
            DISPATCH(Dup_2)
            {
                ip++;
                auto b = vm.pop_data();
                auto a = vm.pop_data();
                vm.push_data(a);
                vm.push_data(b);
                vm.push_data(a);
                vm.push_data(b);
                DISPATCH_NEXT;
            }
            DISPATCH(Swap_1)
            {
                ip++;
                auto b = vm.pop_data();
                auto a = vm.pop_data();
                vm.push_data(b);
                vm.push_data(a);
                DISPATCH_NEXT;
            }
            DISPATCH(Over_1)
            {
                ip++;
                auto a = vm.data_stack[vm.data_top-2];
                vm.push_data(a);
                DISPATCH_NEXT;
            }
            DISPATCH(Drop_1)
            {
                ip++;
                vm.data_top -= 1;
                DISPATCH_NEXT;
            }
            DISPATCH(Drop_2)
            {
                ip++;
                vm.data_top -= 2;
                DISPATCH_NEXT;
            }
            DISPATCH(Drop_3)
            {
                ip++;
                vm.data_top -= 3;
                DISPATCH_NEXT;
            }
            DISPATCH(Drop_4)
            {
                ip++;
                vm.data_top -= 4;
                DISPATCH_NEXT;
            }
            DISPATCH(Drop_N)
            {
                ip++;
                auto n = fetch16(ip);
                ip += sizeof(n);
                vm.data_top -= n;
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Double_m1)
            {
                ip++;
                vm.push_data(-1.0);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Double_0)
            {
                ip++;
                vm.push_data(0.0);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Double_1)
            {
                ip++;
                vm.push_data(1.0);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Double_2)
            {
                ip++;
                vm.push_data(2.0);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Fixnum_m1)
            {
                ip++;
                vm.push_data(-1);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Fixnum_0)
            {
                ip++;
                vm.push_data(0);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Fixnum_1)
            {
                ip++;
                vm.push_data(1);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Fixnum_2)
            {
                ip++;
                vm.push_data(2);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Fixnum_3)
            {
                ip++;
                vm.push_data(3);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Fixnum_4)
            {
                ip++;
                vm.push_data(4);
                DISPATCH_NEXT;
            }
            DISPATCH(Literal_Fixnum_5)
            {
                ip++;
                vm.push_data(5);
                DISPATCH_NEXT;
            }
            DISPATCH(Array_New)
            {
                ip++;
                auto type_token = fetch32(ip);
                ip += sizeof(type_token);
                auto size = vm.pop_data();
                auto array_ref = vm.gc->allocate_array(type_token, size.as_fixnum());
                vm.push_data(array_ref);
                DISPATCH_NEXT;
            }
            DISPATCH(Array_Load_Checked)
            {
                ip++;
                auto idx = vm.pop_data().as_fixnum();
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Array);
                auto array = reinterpret_cast<Malang_Array*>(obj_ref);
                if (idx < 0 || idx >= array->size)
                {
                    vm.panic("array load: index out of bounds. index was %d but array size is %d",
                          idx, array->size);
                }
                vm.push_data(array->data[idx]);
                DISPATCH_NEXT;
            }
            DISPATCH(Array_Store_Checked)
            {
                ip++;
                auto value = vm.pop_data();
                auto idx = vm.pop_data().as_fixnum();
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Array);
                auto array = reinterpret_cast<Malang_Array*>(obj_ref);
                if (idx < 0 || idx >= array->size)
                {
                    vm.panic("array store: index out of bounds. index was %d but array size is %d",
                          idx, array->size);
                }
                array->data[idx] = value;
                DISPATCH_NEXT;
            }
            DISPATCH(Array_Load_Unchecked)
            {
                ip++;
                auto idx = vm.pop_data().as_fixnum();
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Array);
                auto array = reinterpret_cast<Malang_Array*>(obj_ref);
                vm.push_data(array->data[idx]);
                DISPATCH_NEXT;
            }
            DISPATCH(Array_Store_Unchecked)
            {
                ip++;
                auto value = vm.pop_data();
                auto idx = vm.pop_data().as_fixnum();
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Array);
                auto array = reinterpret_cast<Malang_Array*>(obj_ref);
                array->data[idx] = value;
                DISPATCH_NEXT;
            }
            DISPATCH(Array_Length)
            {
                ip++;
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Array);
                auto array = reinterpret_cast<Malang_Array*>(obj_ref);
                vm.push_data(array->size);
                DISPATCH_NEXT;
            }
            DISPATCH(Buffer_New)
            {
                ip++;
                auto size = vm.pop_data().as_fixnum();
                auto buff_ref = vm.gc->allocate_buffer(size);
                vm.push_data(buff_ref);
                DISPATCH_NEXT;
            }
            DISPATCH(Buffer_Copy)
            {
                ip++;
                auto obj_a = vm.pop_data().as_object();
                assert(obj_a->object_tag == Buffer);
                auto buff_a = reinterpret_cast<Malang_Buffer*>(obj_a);
                auto obj_b = vm.gc->allocate_buffer(buff_a->size);
                auto buff_b = reinterpret_cast<Malang_Buffer*>(obj_b);
                memcpy(buff_b->data, buff_a->data, buff_b->size);
                vm.push_data(obj_b);
                DISPATCH_NEXT;
            }
            DISPATCH(Buffer_Load_Checked)
            {
                ip++;
                auto idx = vm.pop_data().as_fixnum();
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Buffer);
                auto buffer = reinterpret_cast<Malang_Buffer*>(obj_ref);
                if (idx < 0 || idx >= buffer->size)
                {
                    vm.panic("buffer load: index out of bounds. index was %d but buffer size is %d",
                          idx, buffer->size);
                }
                vm.push_data(buffer->data[idx]);
                DISPATCH_NEXT;
            }
            DISPATCH(Buffer_Store_Checked)
            {
                ip++;
                auto value = vm.pop_data().as_fixnum();
                auto idx = vm.pop_data().as_fixnum();
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Buffer);
                auto buffer = reinterpret_cast<Malang_Buffer*>(obj_ref);
                if (idx < 0 || idx >= buffer->size)
                {
                    vm.panic("buffer store: index out of bounds. index was %d but buffer size is %d",
                          idx, buffer->size);
                }
                buffer->data[idx] = value;
                DISPATCH_NEXT;
            }
            DISPATCH(Buffer_Load_Unchecked)
            {
                ip++;
                auto idx = vm.pop_data().as_fixnum();
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Buffer);
                auto buffer = reinterpret_cast<Malang_Buffer*>(obj_ref);
                vm.push_data(buffer->data[idx]);
                DISPATCH_NEXT;
            }
            DISPATCH(Buffer_Store_Unchecked)
            {
                ip++;
                auto value = vm.pop_data().as_fixnum();
                auto idx = vm.pop_data().as_fixnum();
                auto obj_ref = vm.pop_data().as_object();
                assert(obj_ref->object_tag == Buffer);
                auto buffer = reinterpret_cast<Malang_Buffer*>(obj_ref);
                buffer->data[idx] = value;
                DISPATCH_NEXT;
            }
            DISPATCH(Buffer_Length)
            {
                ip++;
                auto obj = vm.pop_data().as_object();
                assert(obj->object_tag == Buffer);
                auto buff = reinterpret_cast<Malang_Buffer*>(obj);
                vm.push_data(buff->size);
                DISPATCH_NEXT;
            }
            DISPATCH(Load_String_Constant)
            {
                ip++;
                auto idx = fetch32(ip);
                ip += sizeof(idx);
                auto obj = vm.string_constants_objects[idx];
                vm.push_data(obj);
                DISPATCH_NEXT;
            }
            DISPATCH(Alloc_Object)
            {
                ip++;
                auto type_token = fetch32(ip);
                ip += sizeof(type_token);
                auto obj_ref = vm.gc->allocate_object(type_token);
                vm.push_data(obj_ref);
                DISPATCH_NEXT;
            }
        }
    }

    #if DEBUG_MODE
    } // close try
    catch (...)
    {
        debugger(vm, prev_ins_ip);
    }
    #endif
}
