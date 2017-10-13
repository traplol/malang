#ifndef MALANG_VM_VM_HPP
#define MALANG_VM_VM_HPP

#include <vector>
#include <stdint.h>
#include "nun_boxing.hpp"

using byte = unsigned char;

using Malang_Value = Value<>;

struct Malang_VM
{

    void load_code(const std::vector<byte> &code);
    void run();

    std::vector<byte> code;

    uintptr_t ip;
    uintptr_t globals_top;
    uintptr_t locals_top;
    uintptr_t data_top;
    uintptr_t return_top;

    uintptr_t locals_frames_top;
    uintptr_t locals_frames[256];

    Malang_Value globals[16000];
    Malang_Value locals[16000];
    Malang_Value data_stack[16000];
    uintptr_t return_stack[16000];

};

#endif /* MALANG_VM_VM_HPP */
