#ifndef MALANG_AST_AST_ASSIGN_HPP
#define MALANG_AST_AST_ASSIGN_HPP

#include "ast.hpp"

struct Assign_Node : public Ast_Node
{
    Ast_Node *lhs;
    Ast_Node *rhs;
    ~Assign_Node();
    Assign_Node(Ast_Node *lhs, Ast_Node *rhs)
        : lhs(lhs)
        , rhs(rhs)
    {}

    METADATA_OVERRIDES;
};


#endif /* MALANG_AST_AST_ASSIGN_HPP */
