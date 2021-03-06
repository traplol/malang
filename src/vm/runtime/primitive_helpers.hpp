#ifndef PRIMITIVE_HELPERS_H
#define PRIMITIVE_HELPERS_H

#include <string>
#include "reflection.hpp"
#include "primitive_types.hpp"
struct Type_Map;
struct Bound_Function_Map;
namespace Malang_Runtime
{
    void add_method(Bound_Function_Map &b, Type_Map &m, Type_Info *to_type, const std::string &name, const Types &params, Type_Info *ret_ty, Native_Code code);
    void add_constructor(Bound_Function_Map &b, Type_Map &m, Type_Info *t, Function_Parameters params, Native_Code code);
    void add_bin_op_method(Bound_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *other, Type_Info *ret_ty, Native_Code code);
    void add_una_op_method(Bound_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *ret_ty, Native_Code code);
    Num_Fields_Limit add_field(Type_Info *to_type, const std::string &name, Type_Info *type, bool is_readonly, bool is_private);
}

#endif /* PRIMITIVE_HELPERS_H */
