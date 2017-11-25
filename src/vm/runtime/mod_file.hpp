#ifndef MALANG_VM_RUNTIME_MOD_FILE_HPP
#define MALANG_VM_RUNTIME_MOD_FILE_HPP

struct Module_Map;
struct Type_Map;
struct Bound_Function_Map;

namespace Malang_Runtime
{
    void runtime_mod_file_init(Bound_Function_Map&, Type_Map&, Module_Map&);
}

#endif /* MALANG_VM_RUNTIME_MOD_FILE_HPP */
