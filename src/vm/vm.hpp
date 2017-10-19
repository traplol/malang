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

    void add_local(Malang_Value value);
    void add_global(Malang_Value value);
    void add_data(Malang_Value value);

    void push_locals_frame(uintptr_t frame);
    uintptr_t pop_locals_frame();
    Malang_Value *current_locals();
    Malang_Value get_local(intptr_t n);
    void set_local(intptr_t n, Malang_Value value);

    void push_call_frame(Call_Frame frame);
    Call_Frame pop_call_frame();
    Call_Frame current_call_frame();
    Malang_Value *current_args();
    Malang_Value get_arg(intptr_t n);
    void set_arg(intptr_t n, Malang_Value value);

    void push_globals(Malang_Value value);
    Malang_Value pop_globals();
    void push_locals(Malang_Value value);
    Malang_Value pop_locals();
    void push_data(Malang_Value value);
    Malang_Value pop_data();

};

#endif /* MALANG_VM_VM_HPP */
