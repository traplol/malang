#ifndef MALANG_AST_AST_PRIMITIVES_HPP
#define MALANG_AST_AST_PRIMITIVES_HPP

#include <stdint.h>
#include <string>
#include "ast_value.hpp"

#define DEF_VALUE_NODE(class_name, value_type) \
struct class_name : public Ast_RValue \
{ \
    value_type value; \
    ~class_name(); \
    class_name(value_type const &value) : value(value) {} \
    AST_NODE_OVERRIDES; \
}

DEF_VALUE_NODE(Integer_Node, int64_t);
DEF_VALUE_NODE(Real_Node, double);
DEF_VALUE_NODE(String_Node, std::string);
DEF_VALUE_NODE(Boolean_Node, bool);


#endif /* MALANG_AST_AST_PRIMITIVES_HPP */
