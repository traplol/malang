#ifndef MALANG_VM_RUNTIME_FIXNUM_HPP
#define MALANG_VM_RUNTIME_FIXNUM_HPP

struct Type_Map;
struct Bound_Function_Map;
namespace Malang_Runtime
{
    void runtime_fixnum_init(Bound_Function_Map&, Type_Map&);
}

#endif /* MALANG_VM_RUNTIME_FIXNUM_HPP */
