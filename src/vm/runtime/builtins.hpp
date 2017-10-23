#ifndef MALANG_VM_RUNTIME_BUILTINS_HPP
#define MALANG_VM_RUNTIME_BUILTINS_HPP

#include "primitive_function_map.hpp"
struct Symbol_Map;
struct Type_Map;

namespace Malang_Runtime
{
    void runtime_builtins_init(Primitive_Function_Map&, Type_Map&);
}

#endif /* MALANG_VM_RUNTIME_BUILTINS_HPP */
