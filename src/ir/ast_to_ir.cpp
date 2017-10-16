#include "ast_to_ir.hpp"
#include "../ast/nodes.hpp"
#include "nodes.hpp"

#define NOT_IMPL {printf("Ast_To_IR visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

void Ast_To_IR::visit(Variable_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Assign_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Decl_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Fn_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(List_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Integer_Node&n)
{
    auto fixnum = new IR_Fixnum;
    fixnum->value = n.value;
    tree = fixnum;
}

void Ast_To_IR::visit(Real_Node&n)
{
    auto real = new IR_Double;
    real->value = n.value;
    tree = real;
}

void Ast_To_IR::visit(String_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Boolean_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Logical_Or_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Logical_And_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Inclusive_Or_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Exclusive_Or_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(And_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Equals_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Not_Equals_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Less_Than_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Less_Than_Equals_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Greater_Than_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Greater_Than_Equals_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Left_Shift_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Right_Shift_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Add_Node&n)
{
    Ast_To_IR c;
    auto lhs = c.convert_impl(ir, *n.lhs);
    auto rhs = c.convert_impl(ir, *n.rhs);
    auto add = new IR_B_Add;
    add->lhs = lhs;
    add->rhs = rhs;
    tree = add;
}

void Ast_To_IR::visit(Subtract_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Multiply_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Divide_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Modulo_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Call_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Index_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Field_Accessor_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Negate_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Positive_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Not_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Invert_Node&n)
{
    NOT_IMPL;
}

Malang_IR *Ast_To_IR::convert(Ast_Node &n)
{
    auto ir = new Malang_IR;
    ir->roots.push_back(convert_impl(ir, n));
    return ir;
}

IR_Node *Ast_To_IR::convert_impl(Malang_IR *ir, Ast_Node &n)
{
    tree = nullptr;
    this->ir = ir;
    n.accept(*this);
    return tree;
}
