#include "primitive_helpers.hpp"
#include "reflection.hpp"
#include "../../type_map.hpp"

void Malang_Runtime::add_bin_op_method(Primitive_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *other, Type_Info *ret_ty, Native_Code prim)
{
    const auto is_primitive = true;
    auto fn = m.declare_function({other}, ret_ty, is_primitive);
    auto method = new Method_Info{oper, fn, b.add_primitive(oper, fn, prim)};
    if (!t->add_method(method))
    {
        printf("Couldn't add method `%s' to type `%s'\n", oper.c_str(), t->name().c_str());
        delete method;
    }
}

void Malang_Runtime::add_una_op_method(Primitive_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *ret_ty, Native_Code prim)
{
    const auto is_primitive = true;
    auto fn = m.declare_function({}, ret_ty, is_primitive);
    auto method = new Method_Info{oper, fn, b.add_primitive(oper, fn, prim)};
    if (!t->add_method(method))
    {
        printf("Couldn't add method `%s' to type `%s'\n", oper.c_str(), t->name().c_str());
        delete method;
    }
}
