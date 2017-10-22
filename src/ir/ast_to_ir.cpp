#include <sstream>
#include "ast_to_ir.hpp"
#include "../ast/nodes.hpp"
#include "nodes.hpp"

#define NOT_IMPL {printf("Ast_To_IR visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

#define _return(x) { tree = (x); return; }

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

void Ast_To_IR::visit(Variable_Node &n)
{
    auto symbol = find_symbol(n.name);
    if (!symbol)
    {
        n.src_loc.report("error", "Use of undeclared symbol");
        abort();
    }
    _return(symbol);
}

void Ast_To_IR::visit(Assign_Node &n)
{
    auto value = get<IR_Value>(*n.rhs);
    assert(value);
    auto lval = get<IR_LValue>(*n.lhs);
    assert(lval);
    auto sym = dynamic_cast<IR_Symbol*>(lval);
    if (sym)
    {
        auto assign = new IR_Assignment{n.src_loc, sym, value, sym->scope};
        _return(assign);
    }
    else
    {
        NOT_IMPL;
    }
}

void Ast_To_IR::visit(Decl_Node &n)
{
    assert(n.type);
    assert(n.type->type);
    auto exists = cur_scope->symbols->get_symbol(n.variable_name);
    if (exists)
    {
        n.src_loc.report("error", "Cannot declare variable because it has already been declared");
        exists->src_loc.report("here", "");
        abort();
    }
    auto symbol = cur_scope->symbols->make_symbol(n.variable_name, n.type->type, n.src_loc, cur_symbol_scope);
    if (cur_symbol_scope == Symbol_Scope::Local)
    {
        ++cur_locals_count;
    }
    _return(symbol);
}

void Ast_To_IR::visit(Decl_Assign_Node &n)
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
    auto assign = new IR_Assignment{n.src_loc, variable, value, variable->scope};
    _return(assign);
}

void Ast_To_IR::visit(Decl_Constant_Node &n)
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
    auto assign = new IR_Assignment{n.src_loc, variable, value, variable->scope};
    _return(assign);
}

static std::string label_name_gen()
{
    static volatile size_t i = 0;
    std::stringstream ss;
    ss << "L" << i++;
    return ss.str();
}

void Ast_To_IR::visit(Fn_Node &n)
{
    auto old_scope = cur_symbol_scope;
    auto old_locals_count = cur_locals_count;

    push_scope();
    auto block = ir->labels->make_named_block(label_name_gen(), label_name_gen(), n.src_loc);
    assert(block);
    // Since code can be free (outside of a function), we opt to define a function wherever we
    // are. We don't want that code to accidently start executing so we just create a branch
    // that jumps over the body of the function.
    auto branch_over_body = new IR_Branch{n.src_loc, block->end()};
    ir->roots.push_back(branch_over_body);

    cur_symbol_scope = Symbol_Scope::Argument;
    // @TODO: Handle "this" instance being in arg_0 for methods
    // one way to do this is to implicitly append the symbol "this" to the function args
    for (auto &&it = n.params.rbegin(); it != n.params.rend(); ++it)
    {   // Parameters are accessed from right to left because calls ar evaluated left to right:
        // fn (a, b, c, d)
        //     3  2  1  0
        // call(first(), second, x.third, y.fourth())
        auto p_sym = get<IR_Symbol>(**it);
        assert(p_sym);
        p_sym->is_initialized = true;
    }

    cur_symbol_scope = Symbol_Scope::Local;
    cur_locals_count = 0;
    // Parameters and locals are separated but there still needs to be a way to reference them
    // both in the same scope and a way that easily disallows declaring a variable with the
    // same name as a parameter
    cur_scope->symbols->reset_index();
    for (auto &&b : n.body)
    {
        auto body_node = get(*b);
        block->body().push_back(body_node);
    }
    if (auto last = block->body().back())
    {
        if (dynamic_cast<IR_Return*>(last) == nullptr)
        {
            block->body().push_back(new IR_Return{n.src_loc, {}}); // @FixMe: src_loc should be close curly of function def
        }
    }
    if (cur_locals_count > 0)
    {
        auto num_locals_to_alloc = new IR_Allocate_Locals{n.src_loc, cur_locals_count};
        block->body().insert(block->body().begin(), num_locals_to_alloc);
    }
    ir->roots.push_back(block);
    pop_scope();
    cur_symbol_scope = old_scope;
    cur_locals_count = old_locals_count;
    auto callable = new IR_Callable{n.src_loc, block, n.fn_type};
    _return(callable)
}

void Ast_To_IR::visit(List_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Integer_Node &n)
{
    auto fixnum = new IR_Fixnum{n.src_loc, n.type, static_cast<Fixnum>(n.value)};
    _return(fixnum);
}

void Ast_To_IR::visit(Real_Node &n)
{
    auto real = new IR_Double{n.src_loc, n.type, n.value};
    _return(real);
}

void Ast_To_IR::visit(String_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Boolean_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Logical_Or_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Logical_And_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Inclusive_Or_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Exclusive_Or_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(And_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Equals_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Not_Equals_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Less_Than_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Less_Than_Equals_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Greater_Than_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Greater_Than_Equals_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Left_Shift_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Right_Shift_Node &n)
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
void Ast_To_IR::visit(Add_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Add);
}

void Ast_To_IR::visit(Subtract_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Subtract);
}

void Ast_To_IR::visit(Multiply_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Multiply);
}

void Ast_To_IR::visit(Divide_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Divide);
}

void Ast_To_IR::visit(Modulo_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Modulo);
}

void Ast_To_IR::visit(Call_Node &n)
{
    std::vector<IR_Value*> args;
    if (n.args)
    {
        for (auto &&a : n.args->contents)
        {
            auto val = get<IR_Value>(*a);
            assert(val);
            args.push_back(val);
        }
    }
    auto callee = get<IR_Value>(*n.callee);
    auto call = new IR_Call{n.src_loc, callee, args};
    _return(call);
}

void Ast_To_IR::visit(Index_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Field_Accessor_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Negate_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Positive_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Not_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Invert_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Class_Def_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Type_Node &n)
{
    // @Audit: I don't think this should ever be called, is this a design mistake?
    NOT_IMPL;
}

void Ast_To_IR::visit(Return_Node &n)
{
    assert(n.values); // even an empty return this should not be null...
    std::vector<IR_Value*> values;
    for (auto &&v : n.values->contents)
    {
        auto ret_v = get<IR_Value>(*v);
        assert(ret_v);
        values.push_back(ret_v);
    }
    auto retn = new IR_Return{n.src_loc, values};
    _return(retn);
}

Malang_IR *Ast_To_IR::convert(Ast &ast)
{
    cur_symbol_scope = Symbol_Scope::Global;
    cur_locals_count = 0;
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
        ir = new Malang_IR{types, new Label_Map};
    }
    ir->roots.push_back(get(n));
    return ir;
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
