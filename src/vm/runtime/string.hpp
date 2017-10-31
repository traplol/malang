#ifndef MALANG_VM_RUNTIME_STRING_HPP
#define MALANG_VM_RUNTIME_STRING_HPP

struct Type_Map;
struct Bound_Function_Map;
namespace Malang_Runtime
{
    void runtime_string_init(Bound_Function_Map&, Type_Map&);
}

#endif /* MALANG_VM_RUNTIME_STRING_HPP */
