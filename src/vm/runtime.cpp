#include "runtime.hpp"
#include "runtime/fixnum.hpp"
#include "runtime/double.hpp"
#include "runtime/array_buffer.hpp"
#include "runtime/builtins.hpp"


void Malang_Runtime::init_types(Primitive_Function_Map &b, struct Type_Map &types)
{
    Malang_Runtime::runtime_fixnum_init(b, types);
    Malang_Runtime::runtime_double_init(b, types);
    Malang_Runtime::runtime_buffer_init(b, types);
}
void Malang_Runtime::init_builtins(Primitive_Function_Map &b, struct Type_Map &types)
{
    Malang_Runtime::runtime_builtins_init(b, types);
}
