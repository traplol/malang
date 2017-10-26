#ifndef MALANG_AST_AST_IF_ELSE_HPP
#define MALANG_AST_AST_IF_ELSE_HPP

#include "ast_value.hpp"

struct If_Else_Node : Ast_RValue
{
    ~If_Else_Node();
    If_Else_Node(const Source_Location &src_loc, Ast_Value *condition, const std::vector<Ast_Node*> &consequence, const std::vector<Ast_Node*> &alternative, Type_Info *void_type)
        : Ast_RValue(src_loc)
        , condition(condition)
        , consequence(std::move(consequence))
        , alternative(std::move(alternative))
        , void_type(void_type)
        {}
    AST_NODE_OVERRIDES;
    Ast_Value *condition;
    std::vector<Ast_Node*> consequence;
    std::vector<Ast_Node*> alternative;
    Type_Info *void_type;

    virtual Type_Info *get_type() override;
};

#endif /* MALANG_AST_AST_IF_ELSE_HPP */
