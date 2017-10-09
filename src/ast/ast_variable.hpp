#ifndef MALANG_AST_AST_VARIABLE_HPP
#define MALANG_AST_AST_VARIABLE_HPP

#include "ast.hpp"

struct Variable_Node : public Ast_Node
{
    ~Variable_Node();
    Variable_Node(const std::string &name)
        : name(name)
    {}
    METADATA_OVERRIDES;

    std::string name;
};

#endif /* MALANG_AST_AST_VARIABLE_HPP */
