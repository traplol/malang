#ifndef MALANG_AST_AST_RETURN_HPP
#define MALANG_AST_AST_RETURN_HPP

#include <vector>
#include "ast.hpp"

struct Return_Node : public Ast_Node
{
    ~Return_Node();
    Return_Node(const Source_Location &src_loc, struct List_Node *values)
        : Ast_Node(src_loc)
        , values(values)
        {}

    AST_NODE_OVERRIDES;

    struct List_Node *values;
};

#endif /* MALANG_AST_AST_RETURN_HPP */
