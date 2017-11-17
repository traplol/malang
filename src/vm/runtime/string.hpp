#ifndef MALANG_VM_RUNTIME_STRING_HPP
#define MALANG_VM_RUNTIME_STRING_HPP

#include "primitive_types.hpp"

struct Type_Map;
struct Bound_Function_Map;
namespace Malang_Runtime
{
    void runtime_string_init(Bound_Function_Map&, Type_Map&);
    void string_construct_intern(Malang_Object *place, const String_Constant &string_constant);
    void string_construct_intern(Malang_Object *place, Fixnum size, char *buffer);
    void string_alloc_push(Malang_VM &vm, const String_Constant &string);
}

#endif /* MALANG_VM_RUNTIME_STRING_HPP */
