#ifndef MALANG_AST_AST_VARIABLE_HPP
#define MALANG_AST_AST_VARIABLE_HPP

#include "ast_value.hpp"

struct Variable_Node : public Ast_LValue
{
    ~Variable_Node();
    Variable_Node(const Source_Location &src_loc, const std::string &name)
        : Ast_LValue(src_loc)
        , name(name)
    {}
    AST_NODE_OVERRIDES;

    std::string name;
};

#endif /* MALANG_AST_AST_VARIABLE_HPP */
