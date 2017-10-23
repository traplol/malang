#ifndef MALANG_VM_RUNTIME_DOUBLE_HPP
#define MALANG_VM_RUNTIME_DOUBLE_HPP

#include "primitive_function_map.hpp"
struct Type_Map;
namespace Malang_Runtime
{
    void runtime_double_init(Primitive_Function_Map &, Type_Map&);
}

#endif /* MALANG_VM_RUNTIME_DOUBLE_HPP */
