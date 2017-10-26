#ifndef MALANG_AST_AST_LOOP_HPP
#define MALANG_AST_AST_LOOP_HPP

#include "ast.hpp"
#include "ast_value.hpp"
#include <vector>

struct While_Node : public Ast_Node
{
    ~While_Node();
    While_Node(const Source_Location &src_loc, Ast_Value *condition, const std::vector<Ast_Node*> &body)
        : Ast_Node(src_loc)
        , condition(condition)
        , body(std::move(body))
        {}

    AST_NODE_OVERRIDES;
          
    Ast_Value *condition;
    std::vector<Ast_Node*> body;

};

#endif /* MALANG_AST_AST_LOOP_HPP */
