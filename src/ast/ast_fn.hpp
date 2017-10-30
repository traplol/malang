#ifndef MALANG_AST_AST_FN_HPP
#define MALANG_AST_AST_FN_HPP

#include <vector>

#include "ast.hpp"
#include "ast_decl.hpp"
#include "ast_value.hpp"

struct Fn_Node : Ast_RValue
{
    ~Fn_Node();
    Fn_Node(const Source_Location &src_loc, const std::vector<Decl_Node*> &params, Type_Node *return_type, const Ast_Nodes &body, Function_Type_Info *fn_type)
        : Ast_RValue(src_loc)
        , bound_name()
        , params(std::move(params))
        , return_type(return_type)
        , body(std::move(body))
        , fn_type(fn_type)
        {}
    Fn_Node(const Source_Location &src_loc, const std::string &bound_name, const std::vector<Decl_Node*> &params, Type_Node *return_type, const Ast_Nodes &body, Function_Type_Info *fn_type)
        : Ast_RValue(src_loc)
        , bound_name(bound_name)
        , params(std::move(params))
        , return_type(return_type)
        , body(std::move(body))
        , fn_type(fn_type)
        {}
    virtual Type_Info *get_type() override;
    bool is_bound() const { return !bound_name.empty(); }
    AST_NODE_OVERRIDES;

    std::string bound_name;
    std::vector<Decl_Node*> params;
    Type_Node *return_type;
    Ast_Nodes body;
    Function_Type_Info *fn_type;

};

using Ast_Functions = std::vector<Fn_Node*>;
using Ast_Bound_Functions = Ast_Functions;

#endif /* MALANG_AST_AST_FN_HPP */
