#ifndef MALANG_VM_RUNTIME_RUNTIME_HPP
#define MALANG_VM_RUNTIME_RUNTIME_HPP

#include "runtime/gc.hpp"
#include "runtime/string.hpp"
#include "runtime/double.hpp"
#include "runtime/fixnum.hpp"
#include "runtime/array_buffer.hpp"
#include "../ir/bound_function_map.hpp"
namespace Malang_Runtime
{
    void init_types(Bound_Function_Map &b, Type_Map &types);
    void init_builtins(Bound_Function_Map &b, Type_Map &types);
};

#endif /* MALANG_VM_RUNTIME_RUNTIME_HPP */
