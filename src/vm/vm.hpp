#ifndef MALANG_VM_VM_HPP
#define MALANG_VM_VM_HPP

#include <vector>
#include <stdint.h>
#include "runtime_primitive_types.hpp"
#include "reflection.hpp"

using byte = unsigned char;

struct Call_Frame
{
    uintptr_t return_ip;
    uintptr_t args_frame;
};

struct Malang_VM
{

    void load_code(const std::vector<byte> &code);
    void run();

    std::vector<byte> code;

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

    void push_call_frame(Call_Frame frame);
    Call_Frame pop_call_frame();
    Call_Frame current_call_frame();
    Malang_Value *current_args();

    void push_globals(Malang_Value value);
    Malang_Value pop_globals();
    void push_locals(Malang_Value value);
    Malang_Value pop_locals();
    void push_data(Malang_Value value);
    Malang_Value pop_data();

};

#endif /* MALANG_VM_VM_HPP */
