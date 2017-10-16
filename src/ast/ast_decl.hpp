#ifndef MALANG_AST_AST_DECL_HPP
#define MALANG_AST_AST_DECL_HPP

#include <string>

#include "ast_value.hpp"

struct Decl_Node : public Ast_LValue
{
    std::string variable_name;
    std::string type;
    ~Decl_Node();
    Decl_Node(const std::string &variable, const std::string &type)
        : variable_name(variable)
        , type(type)
    {}

    AST_NODE_OVERRIDES;
};


#endif /* MALANG_AST_AST_DECL_HPP */
