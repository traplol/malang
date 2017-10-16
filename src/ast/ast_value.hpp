#ifndef MALANG_AST_AST_VALUE_HPP
#define MALANG_AST_AST_VALUE_HPP

#include "ast.hpp"

struct Ast_Value : public Ast_Node
{
    virtual void *get_type();
    virtual bool can_lvalue() const = 0;
    virtual bool can_rvalue() const = 0;
};

struct Ast_LValue : public Ast_Value
{
    virtual bool can_lvalue() const final;
    virtual bool can_rvalue() const final;
};

struct Ast_RValue : public Ast_Value
{
    virtual bool can_lvalue() const final;
    virtual bool can_rvalue() const final;
};

#endif /* MALANG_AST_AST_VALUE_HPP */
