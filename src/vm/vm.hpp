#ifndef MALANG_VM_VM_HPP
#define MALANG_VM_VM_HPP

#include <vector>
#include <stdint.h>

using byte = unsigned char;

struct Malang_VM
{
    void load_code(const std::vector<byte> &code);
    void run();

    byte *ip;
    std::vector<byte> code;
    std::vector<intptr_t> globals;
    std::vector<intptr_t> locals;
    std::vector<intptr_t> data_stack;
    std::vector<byte*> return_stack;
};

#endif /* MALANG_VM_VM_HPP */
