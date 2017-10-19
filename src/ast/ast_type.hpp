#ifndef MALANG_AST_AST_TYPE_HPP
#define MALANG_AST_AST_TYPE_HPP

#include "ast.hpp"

struct Type_Node : Ast_Node
{
    Type_Node(const Source_Location &src_loc, struct Type_Info *type)
        : Ast_Node(src_loc)
        , type(type)
        {}
    struct Type_Info *type;
    AST_NODE_OVERRIDES;
};

#endif /* MALANG_AST_AST_TYPE_HPP */
