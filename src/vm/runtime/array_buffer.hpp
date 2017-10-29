#ifndef MALANG_VM_RUNTIME_ARRAY_BUFFER_HPP
#define MALANG_VM_RUNTIME_ARRAY_BUFFER_HPP

#include "primitive_function_map.hpp"
struct Type_Map;

namespace Malang_Runtime
{
    void runtime_buffer_init(Primitive_Function_Map&, Type_Map&);
}

#endif /* MALANG_VM_RUNTIME_ARRAY_BUFFER_HPP */
