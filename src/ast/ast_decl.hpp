#ifndef MALANG_AST_AST_DECL_HPP
#define MALANG_AST_AST_DECL_HPP

#include <string>

#include "ast.hpp"
#include "../type_info.hpp"

struct Decl_Node : public Ast_Node
{
    std::string variable_name;
    Type *type;
    ~Decl_Node();
    Decl_Node(const std::string &variable, Type *type)
        : variable_name(variable)
        , type(type)
    {}

    METADATA_OVERRIDES;
};


#endif /* MALANG_AST_AST_DECL_HPP */
