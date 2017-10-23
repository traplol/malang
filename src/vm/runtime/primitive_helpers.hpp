#ifndef PRIMITIVE_HELPERS_H
#define PRIMITIVE_HELPERS_H

#include <string>
#include "reflection.hpp"
#include "primitive_types.hpp"
#include "primitive_function_map.hpp"
struct Type_Map;

namespace Malang_Runtime
{
    void add_bin_op_method(Primitive_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *other, Type_Info *ret_ty, Native_Code prim);
    void add_una_op_method(Primitive_Function_Map &b, Type_Map &m, Type_Info *t, const std::string &oper, Type_Info *ret_ty, Native_Code prim);
}

#endif /* PRIMITIVE_HELPERS_H */
