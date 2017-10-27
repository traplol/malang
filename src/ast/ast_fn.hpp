#ifndef MALANG_AST_AST_FN_HPP
#define MALANG_AST_AST_FN_HPP

#include <vector>

#include "ast.hpp"
#include "ast_decl.hpp"
#include "ast_value.hpp"

struct Fn_Node : Ast_RValue
{
    ~Fn_Node();
    Fn_Node(const Source_Location &src_loc, const std::vector<Decl_Node*> &params, Type_Node *return_type, const std::vector<Ast_Node*> &body, Function_Type_Info *fn_type)
        : Ast_RValue(src_loc)
        , params(std::move(params))
        , return_type(return_type)
        , body(std::move(body))
        , fn_type(fn_type)
        {}
    virtual Type_Info *get_type() override;
    AST_NODE_OVERRIDES;

    std::vector<Decl_Node*> params;
    Type_Node *return_type;
    std::vector<Ast_Node*> body;
    Function_Type_Info *fn_type;
};

#endif /* MALANG_AST_AST_FN_HPP */
