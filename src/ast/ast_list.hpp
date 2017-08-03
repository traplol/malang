#ifndef MALANG_AST_AST_LIST_HPP
#define MALANG_AST_AST_LIST_HPP

#include <vector>
#include "ast.hpp"

struct List_Node : public Ast_Node
{
    std::vector<Ast_Node*> contents;
    ~List_Node();
    List_Node(const std::vector<Ast_Node*> &contents)
        : contents(std::move(contents))
    {}

    AST_NODE_OVERRIDES;
};

#endif /* MALANG_AST_AST_LIST_HPP */
