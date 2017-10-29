#include <sstream>
#include "ast_to_ir.hpp"
#include "../ast/nodes.hpp"
#include "../vm/runtime/reflection.hpp"
#include "../vm/runtime/primitive_function_map.hpp"
#include "nodes.hpp"

#define NOT_IMPL {printf("Ast_To_IR visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

#define _return(x) { tree = (x); return; }

static inline
bool type_check(const Source_Location &src_loc, const std::vector<IR_Value*> &values, const Function_Parameters &types)
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

Locality::Locality(Malang_IR *ir)
    : symbols(new Symbol_Map{ir})
    , bound_functions(new Bound_Function_Map)
{}
Locality::~Locality()
{
    delete symbols;
    symbols = nullptr;
    delete bound_functions;
    bound_functions = nullptr;
}
bool Locality::any(const std::string &name) const
{
    if (bound_functions->any(name))
    {
        return true;
    }
    if (symbols->get_symbol(name))
    {
        return true;
    }
    return false;
}
Ast_To_IR::~Ast_To_IR()
{
    delete locality;
    locality = nullptr;
}
Ast_To_IR::Ast_To_IR(Primitive_Function_Map *primitives,
                     std::vector<String_Constant> *strings,
                     Type_Map *types)
    : primitives(primitives)
    , types(types)
    , locality(nullptr)
    , ir(nullptr)
    , strings(strings)
{}

void Ast_To_IR::visit(Variable_Node &n)
{
    if (primitives->builtin_exists(n.name))
    {
        if (!cur_call_arg_types)
        {
            n.src_loc.report("error", "Cannot resolve ambiguous type of `%s'", n.name.c_str());
            abort();
        }
        auto prim_fn = primitives->get_builtin(n.name, *cur_call_arg_types);
        if (prim_fn)
        {
            auto callable = ir->alloc<IR_Callable>(n.src_loc, prim_fn->index, prim_fn->fn_type);
            _return(callable);
        }
        else
        {
            std::stringstream ss;
            for (size_t i = 0; i < cur_call_arg_types->size(); ++i)
            {
                ss << (*cur_call_arg_types)[i]->name();
                if (i + 1 < cur_call_arg_types->size())
                    ss << ", ";
            }
            n.src_loc.report("error", "No builtin function `%s' takes arguments with of types `%s'",
                             n.name.c_str(), ss.str().c_str());
            abort();
        }
    }
    else if (locality->bound_functions->any(n.name))
    {
        if (!cur_call_arg_types)
        {
            n.src_loc.report("error", "Cannot resolve ambiguous type of `%s'", n.name.c_str());
            abort();
        }
        auto bound_fn = locality->bound_functions->get_function(n.name, *cur_call_arg_types);
        if (bound_fn.is_valid())
        {
            if (bound_fn.is_primitive())
            {
                auto callable = ir->alloc<IR_Callable>(n.src_loc, bound_fn.primitive()->index,
                                                       bound_fn.fn_type());
                _return(callable);
            }
            else
            {
                auto callable = ir->alloc<IR_Callable>(n.src_loc, bound_fn.code(),
                                                       bound_fn.fn_type());
                _return(callable);
            }
        }
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
        auto assign = ir->alloc<IR_Assignment>(n.src_loc, sym, value, sym->scope);
        _return(assign);
    }
    else if (auto idx = dynamic_cast<IR_Indexable*>(lval))
    {
        auto assign = ir->alloc<IR_Assignment>(n.src_loc, idx, value, Symbol_Scope::None);
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
    //auto exists = locality->symbols->get_symbol(n.variable_name);
    //if (exists)
    if (symbol_already_declared_here(n.variable_name))
    {
        n.src_loc.report("error", "Cannot declare variable because it has already been declared");
        //exists->src_loc.report("here", "");
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
    auto assign = ir->alloc<IR_Assignment>(n.src_loc, variable, value, variable->scope);
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
    auto assign = ir->alloc<IR_Assignment>(n.src_loc, variable, value, variable->scope);
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
    // Use the stack to save state becaue functions can be nested
    auto old_scope = cur_symbol_scope;
    auto old_locals_count = cur_locals_count;
    auto old_fn = cur_fn;
    auto old_returns = all_returns_this_fn;
    if (old_fn)
    {
        n.src_loc.report("NYI", "Support for closures is not yet implemented");
        abort();
    }
    cur_fn = &n;

    // we need some way to store all returns created during this function definition so we can decide
    // whether or not we use the Return_Fast instruction
    std::vector<IR_Return*> returns_this_fn;
    all_returns_this_fn = &returns_this_fn;
    const bool is_void_return = n.fn_type->return_type() == types->get_void();
    push_locality();
    auto fn_body = ir->labels->make_named_block(label_name_gen(), label_name_gen(), n.src_loc);
    assert(fn_body);
    // Since code can be free (outside of a function), we opt to define a function wherever we
    // are. We don't want that code to accidently start executing so we just create a branch
    // that jumps over the body of the function.
    auto branch_over_body = ir->alloc<IR_Branch>(n.src_loc, fn_body->end());

    cur_symbol_scope = Symbol_Scope::Local;
    // @TODO: Handle "this" instance being in Local_0 for methods
    // one way to do this is to implicitly prepend the symbol "this" to the function args
    for (auto &&it = n.params.rbegin(); it != n.params.rend(); ++it)
    {   // Args are pushed on the stack from left to right so they need to be pulled out
        // in the reverse order they were declared.
        auto p_sym = get<IR_Symbol*>(**it);
        assert(p_sym);
        auto assign_arg_to_local = ir->alloc<IR_Assign_Top>(p_sym->src_loc, p_sym, cur_symbol_scope);
        fn_body->body().push_back(assign_arg_to_local);
        p_sym->is_initialized = true;
    }
    convert_body(n.body, fn_body->body());
    if (fn_body->body().empty())
    {
        if (!is_void_return)
        {
            n.return_type->src_loc.report("error", "Empty function cannot return type `%s'",
                                          n.fn_type->return_type()->name().c_str());
            abort();
        }
        auto empty_retn = ir->alloc<IR_Return>(n.src_loc, std::vector<IR_Value*>(), cur_locals_count != 0);
        all_returns_this_fn->push_back(empty_retn);
        fn_body->body().push_back(empty_retn); // @FixMe: src_loc should be close curly of function def
    }
    else if (auto last = fn_body->body().back())
    {
        if (dynamic_cast<IR_Return*>(last) == nullptr)
        {
            if (!is_void_return)
            {
                n.return_type->src_loc.report("error", "Function expected to return type `%s' but it returns nothing",
                                              n.fn_type->return_type()->name().c_str());
                abort();
            }
            auto last_retn = ir->alloc<IR_Return>(n.src_loc, std::vector<IR_Value*>(), cur_locals_count != 0);
            all_returns_this_fn->push_back(last_retn);
            fn_body->body().push_back(last_retn); // @FixMe: src_loc should be close curly of function def
        }
    }
    if (cur_locals_count > 0)
    {
        auto num_locals_to_alloc = ir->alloc<IR_Allocate_Locals>(n.src_loc, cur_locals_count);
        fn_body->body().insert(fn_body->body().begin(), num_locals_to_alloc);
        for (auto &&ret : *all_returns_this_fn)
        {
            ret->should_leave = true;
        }
    }
    ir->roots.push_back(branch_over_body);
    ir->roots.push_back(fn_body);

    pop_locality();

    if (n.is_bound())
    {
        if (!locality->bound_functions->add_function(n.bound_name, n.fn_type, fn_body))
        {
            n.src_loc.report("error", "Cannot bind %s to `%s' because it already exists",
                             n.fn_type->name().c_str(), n.bound_name.c_str());
            abort();
        }
    }

    // Restore the saved state
    cur_symbol_scope = old_scope;
    cur_locals_count = old_locals_count;
    cur_fn = old_fn;
    all_returns_this_fn = old_returns;
    auto callable = ir->alloc<IR_Callable>(n.src_loc, fn_body, n.fn_type, n.is_bound());
    _return(callable)
}

void Ast_To_IR::visit(List_Node &n)
{
    NOT_IMPL;
}

void Ast_To_IR::visit(Integer_Node &n)
{
    auto fixnum = ir->alloc<IR_Fixnum>(n.src_loc, types->get_int(), static_cast<Fixnum>(n.value));
    _return(fixnum);
}

void Ast_To_IR::visit(Real_Node &n)
{
    auto real = ir->alloc<IR_Double>(n.src_loc, types->get_double(), n.value);
    _return(real);
}

void Ast_To_IR::visit(String_Node &n)
{
    auto string = ir->alloc<IR_String>(n.src_loc, types->get_buffer(), strings->size());
    strings->push_back(n.value);
    _return(string);
}

void Ast_To_IR::visit(Boolean_Node &n)
{
    auto boolean = ir->alloc<IR_Boolean>(n.src_loc, types->get_bool(), n.value);
    _return(boolean);
}

void Ast_To_IR::visit(Logical_Or_Node &n)
{

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
    if (cur_true_label)
    {
        block.push_back(ir->alloc<IR_Pop_Branch_If_True>(lhs->src_loc, cur_true_label));
        block.push_back(rhs);
    }
    else
    {
        auto true_label = ir->labels->make_label(label_name_gen(), lhs->src_loc);
        block.push_back(ir->alloc<IR_Branch_If_True_Or_Pop>(lhs->src_loc, true_label));
        block.push_back(rhs);
        block.push_back(true_label);
    }

    auto r_block = ir->alloc<IR_Block>(n.src_loc, block, types->get_bool());
    _return(r_block);
}

void Ast_To_IR::visit(Logical_And_Node &n)
{
    auto true_label = ir->labels->make_label(label_name_gen(), n.lhs->src_loc);
    auto old_true_label = cur_true_label;
    cur_true_label = true_label;
    std::vector<IR_Node*> block;
    auto lhs = get<IR_Value*>(*n.lhs);
    assert(lhs);
    if (lhs->get_type() != types->get_bool())
    {
        lhs->src_loc.report("error", "Expected type `bool' got `%s'",
                            lhs->get_type()->name().c_str());
        abort();
    }
    cur_true_label = old_true_label;
    auto rhs = get<IR_Value*>(*n.rhs);
    assert(rhs);
    if (rhs->get_type() != types->get_bool())
    {
        rhs->src_loc.report("error", "Expected type `bool' got `%s'",
                            rhs->get_type()->name().c_str());
        abort();
    }

    block.push_back(lhs);
    if (cur_false_label)
    {
        block.push_back(ir->alloc<IR_Pop_Branch_If_False>(lhs->src_loc, cur_false_label));
        block.push_back(true_label);
        block.push_back(rhs);
    }
    else
    {
        auto false_label = ir->labels->make_label(label_name_gen(), lhs->src_loc);
        block.push_back(ir->alloc<IR_Branch_If_False_Or_Pop>(lhs->src_loc, false_label));
        block.push_back(true_label);
        block.push_back(rhs);
        block.push_back(false_label);
    }

    auto r_block = ir->alloc<IR_Block>(n.src_loc, block, types->get_bool());
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
    auto bop = ir->alloc<ir_b_class_name>(n.src_loc); \
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
    Types args_types;
    if (n.args)
    {
        for (auto &&a : n.args->contents)
        {
            auto val = get<IR_Value*>(*a);
            assert(val);
            args.push_back(val);
            args_types.push_back(val->get_type());
        }
    }
    auto old_cur_call_arg_types = cur_call_arg_types;
    Function_Parameters fp(args_types);
    cur_call_arg_types = &fp;


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
    auto call = ir->alloc<IR_Call>(n.src_loc, callee, args);
    cur_call_arg_types = old_cur_call_arg_types;
    _return(call);
}

void Ast_To_IR::visit(Index_Node &n)
{
    auto thing = get<IR_Value*>(*n.thing);
    assert(thing);

    auto thing_ty = thing->get_type();
    assert(thing_ty);
    if (thing_ty == ir->types->get_buffer())
    {
        auto index = get<IR_Value*>(*n.subscript);
        auto indexable = ir->alloc<IR_Indexable>(n.src_loc, thing, index, ir->types->get_int());
        _return(indexable);
    }
    else if (auto arr_ty = dynamic_cast<Array_Type_Info*>(thing_ty))
    {
        auto index = get<IR_Value*>(*n.subscript);
        auto indexable = ir->alloc<IR_Indexable>(n.src_loc, thing, index, arr_ty->of_type());
        _return(indexable);
    }
    else
    {
        printf("calling index method for %s not impl yet.\n", thing_ty->name().c_str());
        abort();
    }
}

void Ast_To_IR::visit(Member_Accessor_Node &n)
{
    auto thing = get<IR_Value*>(*n.thing);
    assert(thing);
    auto member_access = ir->alloc<IR_Member_Access>(n.src_loc, thing, n.member->name);
    _return(member_access);
}

// @FixMe: The immediate evaluation of Negate_Node, Positive_Node, Not_Node, and Invert_Node
// should probably be handled in the IR_To_Code converter...
void Ast_To_IR::visit(Negate_Node &n)
{
    if (n.operand->type_id() == Real_Node::_type_id())
    {
        auto real = static_cast<Real_Node*>(n.operand);
        auto evaled = new IR_Double{n.src_loc, ir->types->get_double(), -real->value};
        _return(evaled);
    }
    if (n.operand->type_id() == Integer_Node::_type_id())
    {
        auto integer = static_cast<Integer_Node*>(n.operand);
        auto evaled = new IR_Fixnum{n.src_loc, ir->types->get_int(),
                                     static_cast<Fixnum>(-integer->value)};
        _return(evaled);
    }
    NOT_IMPL;
}

void Ast_To_IR::visit(Positive_Node &n)
{
    if (n.operand->type_id() == Real_Node::_type_id())
    {
        auto real = static_cast<Real_Node*>(n.operand);
        auto evaled = new IR_Double{n.src_loc, ir->types->get_double(), real->value};
        _return(evaled);
    }
    if (n.operand->type_id() == Integer_Node::_type_id())
    {
        auto integer = static_cast<Integer_Node*>(n.operand);
        auto evaled = new IR_Fixnum{n.src_loc, ir->types->get_int(),
                                     static_cast<Fixnum>(integer->value)};
        _return(evaled);
    }
    NOT_IMPL;
}

void Ast_To_IR::visit(Not_Node &n)
{
    if (n.operand->type_id() == Boolean_Node::_type_id())
    {
        auto boolean = static_cast<Boolean_Node*>(n.operand);
        auto evaled = new IR_Boolean{n.src_loc, ir->types->get_int(),
                                     !boolean->value};
        _return(evaled);
    }
    NOT_IMPL;
}

void Ast_To_IR::visit(Invert_Node &n)
{
    if (n.operand->type_id() == Integer_Node::_type_id())
    {
        auto integer = static_cast<Integer_Node*>(n.operand);
        auto negated = new IR_Fixnum{n.src_loc, ir->types->get_int(),
                                     static_cast<Fixnum>(~integer->value)};
        _return(negated);
    }
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
    auto retn = ir->alloc<IR_Return>(n.src_loc, values, cur_locals_count != 0);
    all_returns_this_fn->push_back(retn);
    _return(retn);
}

void Ast_To_IR::convert_body(const std::vector<Ast_Node*> &src, std::vector<IR_Node*> &dst, IR_Value **last_node_as_value)
{
    IR_Node *last_node = nullptr;
    const bool collecting_last_node = last_node_as_value != nullptr;
    for (size_t i = 0; i < src.size(); ++i)
    {
        const bool is_last_iter = i+1 >= src.size();
        auto ast_node = src[i];
        last_node = get(*ast_node);
        assert(last_node);
        dst.push_back(last_node);
        if (auto val = dynamic_cast<IR_Value*>(last_node))
        {
            if (!is_last_iter || !collecting_last_node)
            {
                if (val->get_type() != types->get_void())
                {
                    auto discard = ir->alloc<IR_Discard_Result>(ast_node->src_loc, 1);
                    dst.push_back(discard);
                }
            }
        }
    }
    if (collecting_last_node)
    {
        *last_node_as_value = dynamic_cast<IR_Value*>(last_node);
    }
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
    auto old_cur_true_label = cur_true_label;
    cur_false_label = loop_block->end();
    cur_true_label = loop_block;

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

    auto branch_if_cond_false = ir->alloc<IR_Pop_Branch_If_False>(n.src_loc, loop_block->end());
    block.push_back(branch_if_cond_false);
    // We push here so variables can be declared inside the loop body but cannot
    // be referenced outside of the loop body while still allowing the loop body
    // to access variables declared outside its own scope.
    locality->symbols->push();
    {
        convert_body(n.body, loop_block->body());
        // The end of a while loop is always a branch back to the condition check.
        loop_block->body().push_back(ir->alloc<IR_Branch>(n.src_loc, condition_check_label));
    }
    locality->symbols->pop();

    block.push_back(loop_block);
    auto ret = ir->alloc<IR_Block>(n.src_loc, block, types->get_void());
    cur_false_label = old_cur_false_label;
    cur_true_label = old_cur_true_label;
    _return(ret);
}

static inline
Type_Info *deduce_type(Type_Info *default_type, IR_Value *cons, IR_Value *alt)
{
    if (!cons || !alt)
    {
        return default_type;
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
    auto old_cur_true_label = cur_true_label;
    auto alt_begin_label = ir->labels->make_label(label_name_gen(), n.src_loc);
    auto cons_begin_label = ir->labels->make_label(label_name_gen(), n.src_loc);
    assert(alt_begin_label);
    cur_false_label = alt_begin_label;
    cur_true_label = cons_begin_label;
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
    auto branch_to_alt = ir->alloc<IR_Pop_Branch_If_False>(n.src_loc, alt_begin_label);
    block.push_back(branch_to_alt);
    const bool is_either_empty = n.consequence.empty() || n.alternative.empty();
    IR_Value *last_conseq = nullptr;
    IR_Value *last_altern = nullptr;
    block.push_back(cons_begin_label);
    convert_body(n.consequence, block, (is_either_empty ? nullptr : &last_conseq));
    if (!n.alternative.empty())
    {
        auto end_label = ir->labels->make_label(label_name_gen(), n.src_loc);
        auto branch_to_end = ir->alloc<IR_Branch>(n.src_loc, end_label);
        block.push_back(branch_to_end);
        block.push_back(alt_begin_label);
        convert_body(n.alternative, block, (is_either_empty ? nullptr : &last_altern));
        block.push_back(end_label);
    }
    else
    {
        block.push_back(alt_begin_label);
    }

    auto if_else_type = deduce_type(types->get_void(), last_conseq, last_altern);
    auto ret = ir->alloc<IR_Block>(n.src_loc, block, if_else_type);
    cur_false_label = old_cur_false_label;
    cur_true_label = old_cur_true_label;
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
    auto new_array = ir->alloc<IR_New_Array>(n.src_loc,
                                      n.array_type,
                                      n.of_type->type->type_token(),
                                      size);
    _return(new_array);
}

Malang_IR *Ast_To_IR::convert(Ast &ast)
{
    cur_symbol_scope = Symbol_Scope::Global;
    cur_locals_count = 0;
    cur_fn = nullptr;
    cur_true_label = cur_false_label = nullptr;
    ir = new Malang_IR{types};
    locality = new Locality{ir};
    convert_body(ast.roots, ir->roots);
    return ir;
}

void Ast_To_IR::push_locality()
{
    scopes.push_back(locality);
    locality = new Locality{ir};
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

bool Ast_To_IR::symbol_already_declared_here(const std::string &name)
{
    return locality->any(name);
}

bool Ast_To_IR::symbol_already_declared_anywhere(const std::string &name)
{
    if (locality->any(name))
    {
        return true;
    }
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        if ((*it)->any(name))
        {
            return true;
        }
    }
    return false;
}
