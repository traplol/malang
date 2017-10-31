#include "primitive_helpers.hpp"
#include "reflection.hpp"
#include "../../type_map.hpp"
#include "../../ir/bound_function_map.hpp"

void Malang_Runtime::add_bin_op_method(Bound_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *other, Type_Info *ret_ty, Native_Code prim)
{
    const auto is_native = true;
    auto fn = m.declare_function({other}, ret_ty, is_native);
    if (!b.add_method(t, oper, fn, prim))
    {
        printf("couldn't add method `%s' to type `%s'", oper.c_str(), t->name().c_str());
        abort();
    }
}

void Malang_Runtime::add_una_op_method(Bound_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *ret_ty, Native_Code prim)
{
    const auto is_native = true;
    auto fn = m.declare_function({}, ret_ty, is_native);
    if (!b.add_method(t, oper, fn, prim))
    {
        printf("couldn't add method `%s' to type `%s'", oper.c_str(), t->name().c_str());
        abort();
    }
}

Num_Fields_Limit Malang_Runtime::add_field(Type_Info *to_type, const std::string &name, Type_Info *field_type, bool is_readonly)
{
    auto field = new Field_Info{name, field_type, is_readonly};
    if (!to_type->add_field(field))
    {
        printf("couldn't add field `%s' to type `%s'", name.c_str(), to_type->name().c_str());
        abort();
    }
    return field->index();
}
