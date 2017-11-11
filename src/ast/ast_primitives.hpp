#ifndef MALANG_AST_AST_PRIMITIVES_HPP
#define MALANG_AST_AST_PRIMITIVES_HPP

#include <stdint.h>
#include <string>
#include "ast_value.hpp"

#define DEF_VALUE_NODE(class_name, value_type)                          \
    struct class_name : public Ast_RValue                               \
    {                                                                   \
        value_type value;                                               \
        Type_Info *type;                                                \
        ~class_name();                                                  \
        class_name(const Source_Location &src_loc, value_type const &value, Type_Info *type) \
            : Ast_RValue(src_loc)                                       \
            , value(value)                                              \
            , type(type) {}                                             \
        virtual Type_Info *get_type() final;                            \
        AST_NODE_OVERRIDES;                                             \
    }

DEF_VALUE_NODE(Integer_Node, int64_t);
DEF_VALUE_NODE(Real_Node, double);
DEF_VALUE_NODE(String_Node, std::string);
DEF_VALUE_NODE(Character_Node, char);
DEF_VALUE_NODE(Boolean_Node, bool);


#endif /* MALANG_AST_AST_PRIMITIVES_HPP */
