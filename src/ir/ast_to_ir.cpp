#include "ast_to_ir.hpp"
#include "../ast/nodes.hpp"
#include "nodes.hpp"

#define NOT_IMPL {printf("Ast_To_IR visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

Scope::Scope()
    : labels(new Label_Map)
    , symbols(new Symbol_Map)
{}
Scope::~Scope()
{
    delete labels;
    labels = nullptr;

    delete symbols;
    symbols = nullptr;
}

Ast_To_IR::Ast_To_IR(Type_Map *types)
    : types(types)
    , cur_scope(new Scope)
{}

void Ast_To_IR::visit(Variable_Node&n)
{
    auto symbol = find_symbol(n.name);
    if (!symbol)
    {
        n.src_loc.report("error", "Use of undeclared symbol");
        abort();
    }
    _return(symbol);
}

void Ast_To_IR::visit(Assign_Node&n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Decl_Node&n)
{
    assert(n.type);
    assert(n.type->type);
    bool is_local = !scopes.empty();
    auto symbol = cur_scope->symbols->make_symbol(n.variable_name, n.type->type, n.src_loc, is_local);
    _return(symbol);
}

void Ast_To_IR::visit(Decl_Assign_Node&n)
{
    auto value = get<IR_Value>(*n.value);
    assert(value);
    auto val_ty = value->get_type();
    if (!val_ty)
    {
        n.value->src_loc.report("error", "Could not deduce type.");
        abort();
    }
    if (!n.decl->type)
    {
        n.decl->type = new Type_Node{n.src_loc, val_ty};
    }
    auto variable = get<IR_Symbol>(*n.decl);
    assert(variable);
    auto assign = new IR_Assignment{n.src_loc};
    assign->is_local = variable->is_local;
    assign->lhs = variable;
    assign->rhs = value;
    _return(assign);
}

void Ast_To_IR::visit(Decl_Constant_Node&n)
{
    // @TODO: Decl_Constant_Node needs to generate an IR_Set_Constant
    auto value = get<IR_Value>(*n.value);
    assert(value);
    auto val_ty = value->get_type();
    if (!val_ty)
    {
        n.value->src_loc.report("error", "Could not deduce type.");
        abort();
    }
    if (!n.decl->type)
    {
        n.decl->type = new Type_Node{n.src_loc, val_ty};
    }
    auto variable = get<IR_Symbol>(*n.decl);
    assert(variable);
    auto assign = new IR_Assignment{n.src_loc};
    assign->lhs = variable;
    assign->rhs = value;
    _return(assign);
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
    auto lhs = get<IR_Value>(*n.lhs); \
    if (!lhs) { \
        n.lhs->src_loc.report("error", "Expected a value"); \
        abort();} \
    auto rhs = get<IR_Value>(*n.rhs); \
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

Malang_IR *Ast_To_IR::convert(Ast &ast)
{
    for (auto &&n : ast.roots)
    {
        convert_one(*n);
    }
    return ir;
}

Malang_IR *Ast_To_IR::convert_one(Ast_Node &n)
{
    if (!ir)
    {
        ir = new Malang_IR;
    }
    ir->roots.push_back(get(n));
    return ir;
}

void Ast_To_IR::_return(IR_Node *value)
{
    tree = value;
}

void Ast_To_IR::push_scope()
{
    scopes.push_back(cur_scope);
    cur_scope = new Scope;
}

void Ast_To_IR::pop_scope()
{
    assert(!scopes.empty());
    auto to_free = cur_scope;
    cur_scope = scopes.back();
    scopes.pop_back();
    delete to_free;
}

IR_Symbol *Ast_To_IR::find_symbol(const std::string &name)
{
    if (auto sym = cur_scope->symbols->get_symbol(name))
    {
        return sym;
    }
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        if (auto sym = (*it)->symbols->get_symbol(name))
        {
            return sym;
        }
    }
    return nullptr;
}
