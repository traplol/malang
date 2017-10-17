#ifndef MALANG_AST_AST_DECL_HPP
#define MALANG_AST_AST_DECL_HPP

#include <string>

#include "ast_value.hpp"

struct Decl_Node : public Ast_LValue
{
    std::string variable_name;
    Type_Info *type;
    ~Decl_Node();
    Decl_Node(const Source_Location &src_loc, const std::string &variable, Type_Info *type)
        : Ast_LValue(src_loc)
        , variable_name(variable)
        , type(type)
    {}

    virtual Type_Info *get_type() final;

    AST_NODE_OVERRIDES;
};


#endif /* MALANG_AST_AST_DECL_HPP */
