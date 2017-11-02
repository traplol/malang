#ifndef MALANG_VM_VM_HPP
#define MALANG_VM_VM_HPP

#include <vector>
#include <stdint.h>
#include "runtime/primitive_types.hpp"
#include "runtime/reflection.hpp"
#include "../type_map.hpp"

using byte = unsigned char;

struct Malang_VM
{
    ~Malang_VM();
    Malang_VM(Type_Map *types,
              const std::vector<Native_Code> &natives,
              const std::vector<String_Constant> &string_constants,
              size_t gc_run_interval = 50, size_t max_num_objects = 1000);

    void load_code(const std::vector<byte> &code);
    void run();

    struct Malang_GC *gc;

    std::vector<byte> code;
    std::vector<Native_Code> natives;
    std::vector<String_Constant> string_constants;
    std::vector<Malang_Object*> string_constants_objects;
    Type_Map *types;
    bool breaking;

    uintptr_t locals_frames_top;
    uintptr_t call_frames_top;

    uintptr_t globals_top;
    uintptr_t locals_top;
    uintptr_t data_top;

    static constexpr size_t n_frames = 256;
    static constexpr size_t n_vars = 16000;

    uintptr_t locals_frames[n_frames];
    byte *call_frames[n_frames];

    Malang_Value globals[n_vars];
    Malang_Value locals[n_vars];
    Malang_Value data_stack[n_vars];

    void panic(const char *fmt, ...);

    void dump_code(uintptr_t ip, size_t n, int width=30) const;
    void trace(uintptr_t ip) const;
    void trace_abort(uintptr_t ip, const char *fmt, ...) const;
    void stack_trace() const;

    void add_local(Malang_Value value);
    void add_global(Malang_Value value);
    void add_data(Malang_Value value);

    inline
    void push_locals_frame(uintptr_t frame)
    {
        assert(locals_frames_top+1 < n_frames);
        locals_frames[locals_frames_top++] = frame;
    }
    inline
    uintptr_t pop_locals_frame()
    {
        assert(locals_frames_top > 0);
        auto frame = locals_frames[--locals_frames_top];
        return frame;
    }
    inline
    Malang_Value *current_locals()
    {
        //assert(locals_frames_top > 0);
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
    void push_call_frame(byte *frame)
    {
        assert(call_frames_top+1 < n_frames);
        call_frames[call_frames_top++] = frame;
    }
    inline
    byte *pop_call_frame()
    {
        assert(call_frames_top > 0);
        auto frame = call_frames[--call_frames_top];
        return frame;
    }
    inline
    byte *current_call_frame()
    {
        assert(call_frames_top > 0);
        auto frame = call_frames[call_frames_top-1];
        return frame;
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
    inline
    Malang_Value peek_data(int n = 0)
    {
        auto data = data_stack[data_top-1-n];
        return data;
    }
};

#endif /* MALANG_VM_VM_HPP */
