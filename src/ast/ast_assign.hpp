#ifndef MALANG_AST_AST_ASSIGN_HPP
#define MALANG_AST_AST_ASSIGN_HPP

#include "ast_value.hpp"

struct Assign_Node : public Ast_Node
{
    Ast_LValue *lhs;
    Ast_Value *rhs;
    ~Assign_Node();
    Assign_Node(const Source_Location &src_loc, Ast_LValue *lhs, Ast_Value *rhs)
        : Ast_Node(src_loc)
        , lhs(lhs)
        , rhs(rhs)
    {}

    AST_NODE_OVERRIDES;
};


#endif /* MALANG_AST_AST_ASSIGN_HPP */
