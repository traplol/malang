#include "primitive_helpers.hpp"
#include "reflection.hpp"
#include "../../type_map.hpp"

void Malang_Runtime::add_bin_op_method(Primitive_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *other, Type_Info *ret_ty, Native_Code prim)
{
    const auto is_primitive = true;
    auto fn = m.declare_function({other}, ret_ty, is_primitive);
    std::string method_name = "b_op " + oper;
    auto method = new Method_Info{method_name, fn, b.add_primitive(method_name, fn, prim)};
    if (!t->add_method(method))
    {
        printf("Couldn't add method `%s' to type `%s'\n", method_name.c_str(), t->name().c_str());
        delete method;
    }
}

void Malang_Runtime::add_una_op_method(Primitive_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *ret_ty, Native_Code prim)
{
    const auto is_primitive = true;
    auto fn = m.declare_function({}, ret_ty, is_primitive);
    std::string method_name = "u_op " + oper;
    auto method = new Method_Info{method_name, fn, b.add_primitive(method_name, fn, prim)};
    if (!t->add_method(method))
    {
        printf("Couldn't add method `%s' to type `%s'\n", method_name.c_str(), t->name().c_str());
        delete method;
    }
}
