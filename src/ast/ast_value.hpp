#ifndef MALANG_AST_AST_VALUE_HPP
#define MALANG_AST_AST_VALUE_HPP


#include <stdint.h>
#include <string>
#include "ast.hpp"

#define DEF_VALUE_NODE(class_name, value_type) \
struct class_name : public Ast_Node \
{ \
    value_type value; \
    ~class_name(); \
    class_name(value_type const &value) : value(value) {} \
    METADATA_OVERRIDES; \
}

DEF_VALUE_NODE(Integer_Node, int64_t);
DEF_VALUE_NODE(Real_Node, double);
DEF_VALUE_NODE(String_Node, std::string);
DEF_VALUE_NODE(Boolean_Node, bool);
DEF_VALUE_NODE(Reference_Node, Ast_Node*);


#endif /* MALANG_AST_AST_VALUE_HPP */
