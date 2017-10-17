#ifndef MALANG_AST_AST_LIST_HPP
#define MALANG_AST_AST_LIST_HPP

#include <vector>
#include "ast_value.hpp"

struct List_Node : public Ast_RValue
{
    std::vector<Ast_Value*> contents;
    ~List_Node();
    List_Node(const Source_Location &src_loc, const std::vector<Ast_Value*> &contents)
        : Ast_RValue(src_loc)
        , contents(std::move(contents))
    {}

    AST_NODE_OVERRIDES;
};

#endif /* MALANG_AST_AST_LIST_HPP */
