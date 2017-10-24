#ifndef MALANG_VM_VM_HPP
#define MALANG_VM_VM_HPP

#include <vector>
#include <stdint.h>
#include "runtime/primitive_types.hpp"
#include "runtime/reflection.hpp"

using byte = unsigned char;

struct Call_Frame
{
    uintptr_t return_ip;
    uintptr_t args_frame;
};

struct Malang_VM
{
    Malang_VM(const std::vector<Native_Code> &primitives, size_t gc_run_interval = 50);

    void load_code(const std::vector<byte> &code);
    void run();

    struct Malang_GC *gc;

    std::vector<byte> code;
    std::vector<Native_Code> primitives;

    uintptr_t ip;

    uintptr_t locals_frames_top;
    uintptr_t call_frames_top;

    uintptr_t globals_top;
    uintptr_t locals_top;
    uintptr_t data_top;

    uintptr_t locals_frames[256];
    Call_Frame call_frames[256];

    Malang_Value globals[16000];
    Malang_Value locals[16000];
    Malang_Value data_stack[16000];

    void dump_code(uintptr_t ip, size_t n, int width=30) const;
    void trace() const;
    void trace_abort(const char *fmt, ...) const;
    void stack_trace(uintptr_t n = 16) const;

    void add_local(Malang_Value value);
    void add_global(Malang_Value value);
    void add_data(Malang_Value value);

    inline
    void push_locals_frame(uintptr_t frame)
    {
        locals_frames[locals_frames_top++] = frame;
    }
    inline
    uintptr_t pop_locals_frame()
    {
        auto frame = locals_frames[--locals_frames_top];
        return frame;
    }
    inline
    Malang_Value *current_locals()
    {
        auto frame = locals_frames[locals_frames_top-1];
        return &locals[frame];
    }
    inline
    Malang_Value get_local(intptr_t n)
    {
        return current_locals()[0+n];
    }
    inline
    void set_local(intptr_t n, Malang_Value value)
    {
        current_locals()[0+n] = value;
    }

    inline
    void push_call_frame(Call_Frame frame)
    {
        call_frames[call_frames_top++] = frame;
    }
    inline
    Call_Frame pop_call_frame()
    {
        auto frame = call_frames[--call_frames_top];
        return frame;
    }
    inline
    Call_Frame current_call_frame()
    {
        auto frame = call_frames[call_frames_top-1];
        return frame;
    }
    inline
    Malang_Value *current_args()
    {
        auto frame = call_frames[call_frames_top-1];
        return &data_stack[frame.args_frame];
    }
    inline
    Malang_Value get_arg(intptr_t n)
    {
        return current_args()[0-n];
    }
    inline
    void set_arg(intptr_t n, Malang_Value value)
    {
        current_args()[0-n] = value;
    }

    inline
    void push_globals(Malang_Value value)
    {
        globals[globals_top++] = value;
    }
    inline
    Malang_Value pop_globals()
    {
        auto global = globals[--globals_top];
        return global;
    }
    inline
    void push_locals(Malang_Value value)
    {
        locals[locals_top++] = value;
    }
    inline
    Malang_Value pop_locals()
    {
        auto local = locals[--locals_top];
        return local;
    }
    inline
    void push_data(Malang_Value value)
    {
        data_stack[data_top++] = value;
    }
    inline
    Malang_Value pop_data()
    {
        auto data = data_stack[--data_top];
        return data;
    }
};

#endif /* MALANG_VM_VM_HPP */
