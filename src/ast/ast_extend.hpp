#ifndef MALANG_AST_AST_EXTEND_HPP
#define MALANG_AST_AST_EXTEND_HPP

#include "ast.hpp"
#include "ast_fn.hpp"

struct Extend_Node : Ast_Node
{
    virtual ~Extend_Node() { PRINT_DTOR; }
    Extend_Node(const Source_Location &src_loc, Type_Node *for_type, const Ast_Bound_Functions &body)
        : Ast_Node(src_loc)
        , for_type(for_type)
        , body(std::move(body))
        {}
    AST_NODE_OVERRIDES;

    Type_Node *for_type;
    Ast_Bound_Functions body;
};

#endif /* MALANG_AST_AST_EXTEND_HPP */
