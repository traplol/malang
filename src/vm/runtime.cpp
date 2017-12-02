#include "runtime.hpp"
#include "runtime/fixnum.hpp"
#include "runtime/double.hpp"
#include "runtime/string.hpp"
#include "runtime/array_buffer.hpp"
#include "runtime/builtins.hpp"

#include "runtime/mod_file.hpp"
#include "runtime/mod_socket.hpp"


void Malang_Runtime::init_types(Bound_Function_Map &b, Type_Map &types)
{
    Malang_Runtime::runtime_fixnum_init(b, types);
    Malang_Runtime::runtime_double_init(b, types);
    Malang_Runtime::runtime_buffer_init(b, types);
    Malang_Runtime::runtime_string_init(b, types);
}

void Malang_Runtime::init_builtins(Bound_Function_Map &b, Type_Map &types)
{
    Malang_Runtime::runtime_builtins_init(b, types);
}

void Malang_Runtime::init_modules(Bound_Function_Map &b, Type_Map &types, Module_Map &modules)
{
    Malang_Runtime::runtime_mod_file_init(b, types, modules);
    Malang_Runtime::runtime_mod_socket_init(b, types, modules);
}
