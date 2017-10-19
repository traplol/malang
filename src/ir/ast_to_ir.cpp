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
    auto fixnum = new IR_Fixnum{n.src_loc, n.type};
    fixnum->value = n.value;
    _return(fixnum);
}

void Ast_To_IR::visit(Real_Node&n)
{
    auto real = new IR_Double{n.src_loc, n.type};
    real->value = n.value;
    _return(real);
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

#define BINARY_OP_CONVERT(ir_b_class_name) \
    auto lhs = dynamic_cast<IR_Value*>(get_node(*n.lhs)); \
    if (!lhs) { \
        n.lhs->src_loc.report("error", "Expected a value"); \
        abort();} \
    auto rhs = dynamic_cast<IR_Value*>(get_node(*n.rhs)); \
    if (!rhs) { \
        n.rhs->src_loc.report("error", "Expected a value"); \
        abort();} \
    auto bop = new ir_b_class_name{n.src_loc}; \
    bop->lhs = lhs; \
    bop->rhs = rhs; \
    _return(bop);
void Ast_To_IR::visit(Add_Node&n)
{
    BINARY_OP_CONVERT(IR_B_Add);
}

void Ast_To_IR::visit(Subtract_Node&n)
{
    BINARY_OP_CONVERT(IR_B_Subtract);
}

void Ast_To_IR::visit(Multiply_Node&n)
{
    BINARY_OP_CONVERT(IR_B_Multiply);
}

void Ast_To_IR::visit(Divide_Node&n)
{
    BINARY_OP_CONVERT(IR_B_Divide);
}

void Ast_To_IR::visit(Modulo_Node&n)
{
    BINARY_OP_CONVERT(IR_B_Modulo);
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

void Ast_To_IR::visit(Class_Def_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Type_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Decl_Assign_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Decl_Constant_Node&n)
{
    NOT_IMPL;
}

Malang_IR *Ast_To_IR::convert(Ast_Node &n)
{
    if (!ir)
    {
        ir = new Malang_IR;
    }
    ir->roots.push_back(get_node(n));
    return ir;
}

IR_Node *Ast_To_IR::get_node(Ast_Node &n)
{
    tree = nullptr;
    n.accept(*this);
    return tree;
}

void Ast_To_IR::_return(IR_Node *value)
{
    tree = value;
}
