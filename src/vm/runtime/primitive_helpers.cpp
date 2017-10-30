#include "primitive_helpers.hpp"
#include "reflection.hpp"
#include "../../type_map.hpp"
#include "../../ir/bound_function_map.hpp"

void Malang_Runtime::add_bin_op_method(Bound_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *other, Type_Info *ret_ty, Native_Code prim)
{
    const auto is_native = true;
    auto fn = m.declare_function({other}, ret_ty, is_native);
    b.add_method(t, oper, fn, prim);
}

void Malang_Runtime::add_una_op_method(Bound_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *ret_ty, Native_Code prim)
{
    const auto is_native = true;
    auto fn = m.declare_function({}, ret_ty, is_native);
    b.add_method(t, oper, fn, prim);
}
