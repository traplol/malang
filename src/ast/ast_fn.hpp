#ifndef MALANG_AST_AST_FN_HPP
#define MALANG_AST_AST_FN_HPP

#include <vector>

#include "ast.hpp"
#include "ast_decl.hpp"
#include "ast_value.hpp"

struct Fn_Node : Ast_RValue
{
    std::vector<Decl_Node*> params;
    Ast_Node *return_type;
    std::vector<Ast_Node*> body;
    Fn_Node(const std::vector<Decl_Node*> &params, Ast_Node *return_type, const std::vector<Ast_Node*> &body)
        : params(std::move(params))
        , return_type(return_type)
        , body(std::move(body))
        {}
    ~Fn_Node();
    AST_NODE_OVERRIDES;
};

#endif /* MALANG_AST_AST_FN_HPP */
