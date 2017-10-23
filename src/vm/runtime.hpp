#ifndef MALANG_VM_RUNTIME_RUNTIME_HPP
#define MALANG_VM_RUNTIME_RUNTIME_HPP

#include "runtime/gc.hpp"
#include "runtime/primitive_function_map.hpp"

struct Type_Map;
struct Symbol_Map;
namespace Malang_Runtime
{
    void init_types(Primitive_Function_Map &primitives, Type_Map &types);
    void init_builtins(Primitive_Function_Map &b, struct Type_Map &types);
};

#endif /* MALANG_VM_RUNTIME_RUNTIME_HPP */
