#ifndef MALANG_AST_AST_VALUE_HPP
#define MALANG_AST_AST_VALUE_HPP

#include "ast.hpp"
#include "../type_map.hpp"

struct Ast_Value : public Ast_Node
{
    Ast_Value(const Source_Location &src_loc) : Ast_Node(src_loc) {}
    virtual Type_Info *get_type();
    virtual bool can_lvalue() const = 0;
    virtual bool can_rvalue() const = 0;
};

struct Ast_LValue : public Ast_Value
{
    Ast_LValue(const Source_Location &src_loc) : Ast_Value(src_loc) {}
    virtual bool can_lvalue() const final;
    virtual bool can_rvalue() const final;
};

struct Ast_RValue : public Ast_Value
{
    Ast_RValue(const Source_Location &src_loc) : Ast_Value(src_loc) {}
    virtual bool can_lvalue() const final;
    virtual bool can_rvalue() const final;
};

#endif /* MALANG_AST_AST_VALUE_HPP */
