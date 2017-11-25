#include "primitive_helpers.hpp"
#include "reflection.hpp"
#include "../../type_map.hpp"
#include "../../ir/bound_function_map.hpp"


void Malang_Runtime::add_method(Bound_Function_Map &b, Type_Map &m, Type_Info *to_type, const std::string &name, const Types &params, Type_Info *ret_ty, Native_Code code)
{
    const auto is_native = true;
    auto fn = m.declare_function(params, ret_ty, is_native);
    if (!b.add_method(to_type, name, fn, code))
    {
        printf("couldn't add method `%s' to type `%s'", name.c_str(), to_type->name().c_str());
        abort();
    }
}

void Malang_Runtime::add_bin_op_method(Bound_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *other, Type_Info *ret_ty, Native_Code code)
{
    add_method(b, m , t, oper, {other}, ret_ty, code);
}

void Malang_Runtime::add_una_op_method(Bound_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *ret_ty, Native_Code code)
{
    add_method(b, m, t, oper, {}, ret_ty, code);
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

void Malang_Runtime::add_constructor(Bound_Function_Map &b, Type_Map &m, Type_Info *t, Function_Parameters params, Native_Code code)
{
    const auto is_native = true;
    auto fn = m.declare_function(params.types(), m.get_void(), is_native);
    if (!b.add_constructor(t, fn, code))
    {
        printf("couldn't add constructor to type `%s'", t->name().c_str());
        abort();
    }
}
