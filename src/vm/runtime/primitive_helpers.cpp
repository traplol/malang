#include "primitive_helpers.hpp"
#include "reflection.hpp"
#include "../../type_map.hpp"

void add_bin_op_method(Type_Map *m, Type_Info *t, const std::string &oper, Type_Info *other, Type_Info *ret_ty, Native_Code prim)
{
    auto fn = m->declare_function({other}, ret_ty);
    std::string method_name = "b_op " + oper;
    auto method = new Method_Info{method_name, fn, m->add_primitive(prim)};
    if (!t->add_method(method))
    {
        printf("Couldn't add method `%s' to type `%s'\n", method_name.c_str(), t->name().c_str());
        delete method;
    }
}

void add_una_op_method(Type_Map *m, Type_Info *t, const std::string &oper, Type_Info *ret_ty, Native_Code prim)
{
    auto fn = m->declare_function({}, ret_ty);
    std::string method_name = "u_op " + oper;
    auto method = new Method_Info{method_name, fn, m->add_primitive(prim)};
    if (!t->add_method(method))
    {
        printf("Couldn't add method `%s' to type `%s'\n", method_name.c_str(), t->name().c_str());
        delete method;
    }
}
