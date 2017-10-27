#include <sstream>
#include "ast_to_ir.hpp"
#include "../ast/nodes.hpp"
#include "../vm/runtime/reflection.hpp"
#include "../vm/runtime/primitive_function_map.hpp"
#include "nodes.hpp"

#define NOT_IMPL {printf("Ast_To_IR visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

#define _return(x) { tree = (x); return; }

static inline
bool type_check(const Source_Location &src_loc, const std::vector<IR_Value*> &values, const std::vector<Type_Info*> &types)
{
    if (values.size() != types.size())
    {
        src_loc.report("error", "Invalid number of values: expected %d got: %d",
                       (int)types.size(), (int)values.size()); // @FixMe: type specifier+cast
        return false;
    }
    for (size_t i = 0; i < values.size(); ++i)
    {
        auto a_t = values[i]->get_type();
        auto e_t = types[i];

        if (!a_t->is_assignable_to(e_t))
        {
            values[i]->src_loc.report("error", "Type mismatch: cannot convert from type `%s' to `%s'",
                                      a_t->name().c_str(), e_t->name().c_str());
            return false;
        }
    }
    return true;
}

Locality::Locality()
    : symbols(new Symbol_Map)
{}
Locality::~Locality()
{
    delete symbols;
    symbols = nullptr;
}
Ast_To_IR::~Ast_To_IR()
{
    delete locality;
    locality = nullptr;
}
Ast_To_IR::Ast_To_IR(Primitive_Function_Map *primitives, Type_Map *types)
    : primitives(primitives)
    , types(types)
    , locality(new Locality)
    , ir(nullptr)
{}

void Ast_To_IR::visit(Variable_Node &n)
{
    if (auto prim_fn = primitives->get_builtin(n.name))
    {
        auto callable = new IR_Callable{n.src_loc, prim_fn->index, prim_fn->fn_type};
        _return(callable);
    }
    auto symbol = find_symbol(n.name);
    if (!symbol)
    {
        n.src_loc.report("error", "Use of undeclared symbol `%s'", n.name.c_str());
        abort();
    }
    _return(symbol);
}

void Ast_To_IR::visit(Assign_Node &n)
{
    auto value = get<IR_Value*>(*n.rhs);
    assert(value);
    auto lval = get<IR_LValue*>(*n.lhs);
    assert(lval);
    if (auto sym = dynamic_cast<IR_Symbol*>(lval))
    {
        auto assign = new IR_Assignment{n.src_loc, sym, value, sym->scope};
        _return(assign);
    }
    else if (auto idx = dynamic_cast<IR_Indexable*>(lval))
    {
        auto assign = new IR_Assignment{n.src_loc, idx, value, Symbol_Scope::None};
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
    auto exists = locality->symbols->get_symbol(n.variable_name);
    if (exists)
    {
        n.src_loc.report("error", "Cannot declare variable because it has already been declared");
        exists->src_loc.report("here", "");
        abort();
    }
    auto symbol = locality->symbols->make_symbol(n.variable_name, n.type->type, n.src_loc, cur_symbol_scope);
    if (cur_symbol_scope == Symbol_Scope::Local)
    {
        ++cur_locals_count;
    }
    _return(symbol);
}

void Ast_To_IR::visit(Decl_Assign_Node &n)
{
    auto value = get<IR_Value*>(*n.value);
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
    auto variable = get<IR_Symbol*>(*n.decl);
    assert(variable);
    variable->is_initialized = true;
    auto assign = new IR_Assignment{n.src_loc, variable, value, variable->scope};
    _return(assign);
}

void Ast_To_IR::visit(Decl_Constant_Node &n)
{
    // @TODO: Decl_Constant_Node needs to generate an IR_Set_Constant
    auto variable = get<IR_Symbol*>(*n.decl);
    assert(variable);
    variable->is_initialized = true;
    auto value = get<IR_Value*>(*n.value);
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
    auto assign = new IR_Assignment{n.src_loc, variable, value, variable->scope};
    _return(assign);
}

static std::string label_name_gen()
{
    static volatile size_t i = 0;
    std::stringstream ss;
    ss << "L" << i++; // @FixMe: atomic increment.
    return ss.str();
}

void Ast_To_IR::visit(Fn_Node &n)
{
    auto old_scope = cur_symbol_scope;
    auto old_locals_count = cur_locals_count;
    auto old_fn = cur_fn;
    if (old_fn)
    {
        n.src_loc.report("NYI", "Support for closures is not yet implemented");
        abort();
    }
    cur_fn = &n;

    push_locality();
    auto fn_body = ir->labels->make_named_block(label_name_gen(), label_name_gen(), n.src_loc);
    assert(fn_body);
    // Since code can be free (outside of a function), we opt to define a function wherever we
    // are. We don't want that code to accidently start executing so we just create a branch
    // that jumps over the body of the function.
    auto branch_over_body = new IR_Branch{n.src_loc, fn_body->end()};

    cur_symbol_scope = Symbol_Scope::Local;
    // @TODO: Handle "this" instance being in arg_0 for methods
    // one way to do this is to implicitly append the symbol "this" to the function args
    for (auto &&it = n.params.rbegin(); it != n.params.rend(); ++it)
    {   // Args are pushed on the stack from left to right so they need to be pulled out
        // in the reverse order they were declared.
        auto p_sym = get<IR_Symbol*>(**it);
        assert(p_sym);
        auto assign_arg_to_local = new IR_Assign_Top{p_sym->src_loc, p_sym, cur_symbol_scope};
        fn_body->body().push_back(assign_arg_to_local);
        //p_sym->is_initialized = true;
    }

    cur_symbol_scope = Symbol_Scope::Local;
    //cur_locals_count = 0;
    // Parameters and locals are separated but there still needs to be a way to reference them
    // both in the same scope and a way that easily disallows declaring a variable with the
    // same name as a parameter
    //locality->symbols->reset_index();
    for (auto &&b : n.body)
    {
        auto body_node = get(*b);
        fn_body->body().push_back(body_node);
    }
    if (fn_body->body().empty())
    {
        fn_body->body().push_back(new IR_Return{n.src_loc, {}, cur_locals_count != 0}); // @FixMe: src_loc should be close curly of function def
    }
    else if (auto last = fn_body->body().back())
    {
        if (dynamic_cast<IR_Return*>(last) == nullptr)
        {
            fn_body->body().push_back(new IR_Return{n.src_loc, {}, cur_locals_count != 0}); // @FixMe: src_loc should be close curly of function def
        }
    }
    if (cur_locals_count > 0)
    {
        auto num_locals_to_alloc = new IR_Allocate_Locals{n.src_loc, cur_locals_count};
        fn_body->body().insert(fn_body->body().begin(), num_locals_to_alloc);
    }
    ir->roots.push_back(branch_over_body);
    ir->roots.push_back(fn_body);
    pop_locality();
    cur_symbol_scope = old_scope;
    cur_locals_count = old_locals_count;
    cur_fn = old_fn;
    auto callable = new IR_Callable{n.src_loc, fn_body, n.fn_type};
    _return(callable)
}

void Ast_To_IR::visit(List_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Integer_Node &n)
{
    auto fixnum = new IR_Fixnum{n.src_loc, types->get_int(), static_cast<Fixnum>(n.value)};
    _return(fixnum);
}

void Ast_To_IR::visit(Real_Node &n)
{
    auto real = new IR_Double{n.src_loc, types->get_double(), n.value};
    _return(real);
}

void Ast_To_IR::visit(String_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Boolean_Node &n)
{
    auto boolean = new IR_Boolean{n.src_loc, types->get_bool(), n.value};
    _return(boolean);
}

void Ast_To_IR::visit(Logical_Or_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Logical_And_Node &n)
{
    // 1 < 2 && 10 < 100
    //
    // Lit(1)
    // Lit(1)
    // Less_Than
    // Dup_1
    // Branch_If_False(exit)
    // Lit(10)
    // Lit(100)
    // Less_Than
    // Label(exit)

    std::vector<IR_Node*> block;
    auto lhs = get<IR_Value*>(*n.lhs);
    assert(lhs);
    if (lhs->get_type() != types->get_bool())
    {
        lhs->src_loc.report("error", "Expected type `bool' got `%s'",
                            lhs->get_type()->name().c_str());
        abort();
    }

    auto rhs = get<IR_Value*>(*n.rhs);
    assert(rhs);
    if (rhs->get_type() != types->get_bool())
    {
        rhs->src_loc.report("error", "Expected type `bool' got `%s'",
                            rhs->get_type()->name().c_str());
        abort();
    }

    block.push_back(lhs);
    IR_Label *short_circuit_label;
    if (cur_false_label)
    {
        short_circuit_label = cur_false_label;
    }
    else
    {
        short_circuit_label = ir->labels->make_label(label_name_gen(), n.src_loc);
    }
    auto short_circuit = new IR_Branch_If_False{n.src_loc, short_circuit_label};
    block.push_back(short_circuit);
    block.push_back(rhs);

    auto r_block = new IR_Block{n.src_loc, block, types->get_bool()};
    _return(r_block);
}


#define BINARY_OP_CONVERT(ir_b_class_name) \
    auto lhs = get<IR_Value*>(*n.lhs); \
    if (!lhs) { \
        n.lhs->src_loc.report("error", "Expected a value"); \
        abort();} \
    auto rhs = get<IR_Value*>(*n.rhs); \
    if (!rhs) { \
        n.rhs->src_loc.report("error", "Expected a value"); \
        abort();} \
    auto bop = new ir_b_class_name{n.src_loc}; \
    bop->lhs = lhs; \
    bop->rhs = rhs; \
    _return(bop);
void Ast_To_IR::visit(Inclusive_Or_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Or);
}

void Ast_To_IR::visit(Exclusive_Or_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Xor);
}

void Ast_To_IR::visit(And_Node &n)
{
    BINARY_OP_CONVERT(IR_B_And);
}

void Ast_To_IR::visit(Equals_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Equals);
}

void Ast_To_IR::visit(Not_Equals_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Not_Equals);
}

void Ast_To_IR::visit(Less_Than_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Less_Than);
}

void Ast_To_IR::visit(Less_Than_Equals_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Less_Than_Equals);
}

void Ast_To_IR::visit(Greater_Than_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Greater_Than);
}

void Ast_To_IR::visit(Greater_Than_Equals_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Greater_Than_Equals);
}

void Ast_To_IR::visit(Left_Shift_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Left_Shift);
}

void Ast_To_IR::visit(Right_Shift_Node &n)
{
    BINARY_OP_CONVERT(IR_B_Right_Shift);
}
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
            auto val = get<IR_Value*>(*a);
            assert(val);
            args.push_back(val);
        }
    }
    auto callee = get<IR_Value*>(*n.callee);
    assert(callee);
    auto fn_type = dynamic_cast<Function_Type_Info*>(callee->get_type());
    if (!fn_type)
    {
        n.src_loc.report("error", "Attempted to call non-function type `%s'",
                 callee->get_type()->name().c_str());
        abort();
    }
    if (!type_check(n.src_loc, args, fn_type->parameter_types()))
    {
        abort();
    }
    auto call = new IR_Call{n.src_loc, callee, args};
    _return(call);
}

void Ast_To_IR::visit(Index_Node &n)
{
    auto thing = get<IR_Value*>(*n.thing);
    assert(thing);

    auto thing_ty = thing->get_type();
    assert(thing_ty);
    if (auto arr_ty = dynamic_cast<Array_Type_Info*>(thing_ty))
    {
        auto index = get<IR_Value*>(*n.subscript);
        auto indexable = new IR_Indexable{n.src_loc, thing, index, arr_ty->of_type()};
        _return(indexable);
    }
    else
    {
        printf("calling index method not impl yet.\n");
        abort();
    }
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

    if (!cur_fn)
    {
        n.src_loc.report("error", "return outside of a function does not make sense.");
        abort();
    }
    if (n.values->contents.size() > 1)
    { // @FixMe: implement sum types or a similar mechanism to type check multiple return values
        n.src_loc.report("not implemented", "multiple return types not yet implemented.");
        abort();
    }

    std::vector<IR_Value*> values;
    // None
    if (n.values->contents.empty()
        && cur_fn->fn_type->return_type() != ir->types->get_void())
    {
        n.src_loc.report("error", "Expected value to return in non-void function");
        abort();
    }
    // 1
    if (n.values->contents.size() == 1)
    {
        auto v = n.values->contents[0];
        auto ret_v = get<IR_Value*>(*v);
        assert(ret_v);
        if (!ret_v->get_type()->is_assignable_to(cur_fn->fn_type->return_type()))
        {
            n.src_loc.report("error", "Function expecting to return type `%s' cannot convert type `%s'",
                             cur_fn->fn_type->return_type()->name().c_str(),
                             ret_v->get_type()->name().c_str());
            abort();
        }
        else
        {
            values.push_back(ret_v);
        }
    }

    /* 
    // Multi:
    for (auto &&v : n.values->contents)
    {
        auto ret_v = get<IR_Value*>(*v);
        assert(ret_v);
        values.push_back(ret_v);
    }
    */
    auto retn = new IR_Return{n.src_loc, values, cur_locals_count != 0};
    _return(retn);
}

void Ast_To_IR::visit(While_Node &n)
{
    std::vector<IR_Node*> block;
    auto condition_check_label = ir->labels->make_label(label_name_gen(), n.src_loc);
    assert(condition_check_label);
    block.push_back(condition_check_label);
    auto loop_block = ir->labels->make_named_block(label_name_gen(), label_name_gen(), n.src_loc);
    assert(loop_block);
    auto old_cur_false_label = cur_false_label;
    cur_false_label = loop_block->end();

    assert(n.condition);
    auto cond = get<IR_Value*>(*n.condition);
    assert(cond);
    if (cond->get_type() != types->get_bool())
    {
        n.condition->src_loc.report("error", "Conditional for a while statement must be type `bool', got `%s' instead",
                                    cond->get_type()->name().c_str());
        abort();
    }
    block.push_back(cond);

    auto branch_if_cond_false = new IR_Branch_If_False{n.src_loc, loop_block->end()};
    block.push_back(branch_if_cond_false);
    // We push here so variables can be declared inside the loop body but cannot
    // be referenced outside of the loop body while still allowing the loop body
    // to access variables declared outside its own scope.
    locality->symbols->push();
    {
        for (auto &&b : n.body)
        {
            auto body_node = get(*b);
            assert(body_node);
            loop_block->body().push_back(body_node);
        }
        // The end of a while loop is always a branch back to the condition check.
        loop_block->body().push_back(new IR_Branch{n.src_loc, condition_check_label});
    }
    locality->symbols->pop();

    block.push_back(loop_block);
    auto ret = new IR_Block{n.src_loc, block, types->get_void()};
    cur_false_label = old_cur_false_label;
    _return(ret);
}

static inline
Type_Info *deduce_type(Type_Info *default_type, IR_Value *cons, IR_Value *alt)
{
    if (!cons && !alt)
    {
        return default_type;
    }
    if (!cons && alt)
    {
        printf("TODO: !a && b\n");
        abort();
    }
    if (cons && !alt)
    {
        printf("TODO: a && !b\n");
        abort();
    }
    auto cons_ty = cons->get_type();
    assert(cons_ty);
    auto alt_ty = alt->get_type();
    assert(alt_ty);
    if (alt_ty->is_assignable_to(cons_ty))
    {
        return cons_ty;
    }

    alt->src_loc.report("error", "Cannot convert from type `%s' to `%s'",
                        alt_ty->name().c_str(), cons_ty->name().c_str());
    abort();
}

void Ast_To_IR::visit(If_Else_Node &n)
{
    auto old_cur_false_label = cur_false_label;
    auto alt_begin_label = ir->labels->make_label(label_name_gen(), n.src_loc);
    assert(alt_begin_label);
    cur_false_label = alt_begin_label;
    std::vector<IR_Node*> block;
    assert(n.condition);
    auto cond = get<IR_Value*>(*n.condition);
    assert(cond);
    if (cond->get_type() != types->get_bool())
    {
        n.condition->src_loc.report("error", "Conditional for an if/else must be type `bool', got `%s' instead",
                                    cond->get_type()->name().c_str());
        abort();
    }
    block.push_back(cond);
    auto branch_to_alt = new IR_Branch_If_False{n.src_loc, alt_begin_label};
    block.push_back(branch_to_alt);
    IR_Value *last_conseq = nullptr;
    IR_Value *last_altern = nullptr;
    for (auto &&c : n.consequence)
    {
        auto cons_node = get(*c);
        assert(cons_node);
        block.push_back(cons_node);
        last_conseq = dynamic_cast<IR_Value*>(cons_node);
    }
    if (!n.alternative.empty())
    {
        auto end_label = ir->labels->make_label(label_name_gen(), n.src_loc);
        auto branch_to_end = new IR_Branch{n.src_loc, end_label};
        block.push_back(branch_to_end);
        block.push_back(alt_begin_label);
        for (auto &&a : n.alternative)
        {
            auto alt_node = get(*a);
            assert(alt_node);
            block.push_back(alt_node);
            last_altern = dynamic_cast<IR_Value*>(alt_node);
        }
        block.push_back(end_label);
    }
    else
    {
        block.push_back(alt_begin_label);
    }

    auto ret = new IR_Block{n.src_loc, block, deduce_type(types->get_void(), last_conseq, last_altern)};
    cur_false_label = old_cur_false_label;
    _return(ret);
}

void Ast_To_IR::visit(struct Array_Literal_Node &n)
{
    NOT_IMPL;
}
void Ast_To_IR::visit(struct New_Array_Node &n)
{
    auto size = get<IR_Value*>(*n.size);
    assert(size);
    auto size_ty = size->get_type();
    if (!size_ty)
    {
        n.size->src_loc.report("error", "Could not deduce type of array size");
        abort();
    }
    if (!size_ty->is_assignable_to(types->get_int()))
    {
        size->src_loc.report("error", "Array size must be an integer type, got `%s'",
                             size_ty->name().c_str());
        abort();
    }
    auto new_array = new IR_New_Array{n.src_loc,
                                      n.array_type,
                                      n.of_type->type->type_token(),
                                      size};
    _return(new_array);
}

Malang_IR *Ast_To_IR::convert(Ast &ast)
{
    cur_symbol_scope = Symbol_Scope::Global;
    cur_locals_count = 0;
    cur_fn = nullptr;
    cur_false_label = nullptr;
    ir = new Malang_IR{types};
    for (auto &&n : ast.roots)
    {
        auto node = get(*n);
        ir->roots.push_back(node);
    }
    return ir;
}

void Ast_To_IR::push_locality()
{
    scopes.push_back(locality);
    locality = new Locality;
}

void Ast_To_IR::pop_locality()
{
    assert(!scopes.empty());
    auto to_free = locality;
    locality = scopes.back();
    scopes.pop_back();
    delete to_free;
}

IR_Symbol *Ast_To_IR::find_symbol(const std::string &name)
{
    if (auto sym = locality->symbols->get_symbol(name))
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
