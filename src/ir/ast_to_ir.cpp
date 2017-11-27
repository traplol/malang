#include <sstream>
#include "ast_to_ir.hpp"
#include "nodes.hpp"
#include "../defer.hpp"
#include "../platform.hpp"
#include "../parser.hpp"
#include "../ast/nodes.hpp"
#include "../vm/runtime/reflection.hpp"
#include "../visitors/ast_pretty_printer.hpp"

#define NOT_IMPL {printf("Ast_To_IR visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

#define _return(x) { this->__tree = (x); return; }

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

void Ast_To_IR::visit(Import_Node &n)
{
    /*
     * Imports/modules are a mess. 
     */


    assert(n.mod_info);
    if (n.mod_info->color() != n.mod_info->white)
    {
        // nothing to do, already loaded.
        _return(nullptr);
    }
    if (n.mod_info->builtin())
    {   // builtin modules don't need anything processed to load.
        n.mod_info->color(n.mod_info->black);
        _return(nullptr);
    }

    // Save everything
    auto old_cur_module = cur_module;
    auto old_is_extending = is_extending;
    auto old_cur_fn = cur_fn;
    auto old_cur_fn_ep = cur_fn_ep;
    auto old_cur_true_label = cur_true_label;
    auto old_cur_false_label = cur_false_label;
    auto old_cur_continue_label = cur_continue_label;
    auto old_cur_break_label = cur_break_label;
    auto old_locality = locality;
    auto old_cur_symbol_scope = cur_symbol_scope;
    defer({cur_module = old_cur_module;
            is_extending = old_is_extending;
            cur_fn = old_cur_fn;
            cur_fn_ep = old_cur_fn_ep;
            cur_true_label = old_cur_true_label;
            cur_false_label = old_cur_false_label;
            cur_continue_label = old_cur_continue_label;
            cur_break_label = old_cur_break_label;
            locality = old_locality;
            cur_symbol_scope = old_cur_symbol_scope;
            ir->types->module(old_cur_module);
        });

    n.mod_info->color(n.mod_info->grey);
    std::string filename, rel_path;
    // @TODO: move realpath to plat::
    auto this_source_file = realpath(n.src_loc.filename.c_str(), NULL);
    auto this_source_dir = plat::get_directory(this_source_file);
    if (noisy)
    {
        printf("from: %s\n", this_source_file);
        printf("dir: %s\n", this_source_dir.c_str());
    }
    free(this_source_file);
    if (!mod_map->find_file_rel(this_source_dir, n.mod_info, filename))
    {
        if (!mod_map->find_file(n.mod_info, filename))
        {
            n.src_loc.report("error", "Cannot find file to import.");
            abort();
        }
    }
    if (noisy)
    {
        printf("importing %s\n", filename.c_str());
    }
    if (n.mod_info->locality())
    {
        locality = n.mod_info->locality();
    }
    //else
    //{
    //    // ??
    //}
    cur_module = n.mod_info;
    ir->types->module(n.mod_info);
    Parser parser(ir->types, mod_map);
    auto src = new Source_Code(filename); // @Leak
    auto ast = parser.parse(src);
    if (parser.errors)
    {
        n.src_loc.report("error", "error parsing module.");
        abort();
    }
    if (noisy)
    {
        Ast_Pretty_Printer pp;
        auto x = pp.to_strings(ast);
        for (auto &&s : x) {
            printf("%s\n", s.c_str());
        }
    }
    convert_intern(ast);
    n.mod_info->color(n.mod_info->black);
    
    _return(nullptr); // necessary?
}

static
IR_Callable *get_bound(Malang_IR *ir, Scope_Lookup *loc, const Source_Location &src_loc, const std::string &name, Function_Parameters *arg_types)
{
    if (auto bound_fn = loc->current().find_bound_function(name, *arg_types))
    {
        if (bound_fn->is_native())
        {
            auto callable =
                ir->alloc<IR_Callable>(src_loc, bound_fn->native()->index, bound_fn->fn_type());
            return(callable);
        }
        else
        {
            auto callable =
                ir->alloc<IR_Callable>(src_loc, bound_fn->code(), bound_fn->fn_type());
            return(callable);
        }
    }
    return nullptr;
}

void Ast_To_IR::visit(Variable_Node &n)
{
    auto loc = locality;
    if (n.is_qualified())
    {
        auto m = mod_map->get_existing(n.qualifiers());
        if (!m || !m->loaded())
        {
            n.src_loc.report("error", "No imported module matches for `%s'",
                             n.name().c_str());
            abort();
        }
        loc = m->locality();
    }

    if (cur_fn && n.name() == "recurse")
    {
        assert(cur_fn_ep);
        auto callable = ir->alloc<IR_Callable>(n.src_loc, cur_fn_ep, cur_fn->fn_type);
        _return(callable);
    }
    if (cur_call_arg_types)
    {
        if (is_extending)
        {
            if (auto method = is_extending->get_method(n.name(), *cur_call_arg_types))
            {
                auto meth_call =
                    ir->alloc<IR_Call_Method>(n.src_loc, nullptr, method, std::vector<IR_Value*>());
                _return(meth_call);
            }
        }
        if (auto in_this_module = get_bound(ir, loc, n.src_loc, n.name(), cur_call_arg_types))
        {
            _return(in_this_module);
        }
        if (auto global_func = get_bound(ir, globals, n.src_loc, n.name(), cur_call_arg_types))
        {
            _return(global_func);
        }
        if (auto type = ir->types->get_type(n.name()))
        {   // Calling constructor
            auto alloc = ir->alloc<IR_Allocate_Object>(n.src_loc, type);
            _return(alloc);
        }

    }
    auto symbol = find_symbol(n.name());
    if (symbol)
    {
        _return(symbol);
    }
    // Any correct paths have "_return"ed by this point so it's an error...
    if (cur_call_arg_types)
    {
        n.src_loc.report("error", "No function matches `fn %s%s'",
                         n.name().c_str(), cur_call_arg_types->to_string().c_str());
        abort();
    }
    else
    {
        n.src_loc.report("error", "Use of undeclared symbol `%s'", n.name().c_str());
        abort();
    }
}

void Ast_To_IR::visit(Assign_Node &n)
{
    auto value = get<IR_Value*>(*n.rhs);
    assert(value);
    auto lval = get<IR_LValue*>(*n.lhs);
    if (!lval)
    {
        n.src_loc.report("error", "LHS of assign is not an lvalue");
        abort();
    }

    if (!value->get_type()->is_assignable_to(lval->get_type()))
    {
        n.src_loc.report("error", "Cannot assign from type `%s' to `%s'",
                         value->get_type()->name().c_str(),
                         lval->get_type()->name().c_str());
        abort();
    }
        
    if (auto sym = dynamic_cast<IR_Symbol*>(lval))
    {
        sym->is_initialized = true;
    }

    if (auto sym = dynamic_cast<IR_Symbol*>(lval))
    {
        if (sym->is_initialized && sym->is_readonly)
        {
            n.src_loc.report("error", "`%s' is constant and cannot be reassigned",
                             sym->symbol.c_str());
            abort();
        }
        auto assign = ir->alloc<IR_Assignment>(n.src_loc, sym, value, sym->scope);
        _return(assign);
    }
    else if (auto idx = dynamic_cast<IR_Indexable*>(lval))
    {
        auto assign = ir->alloc<IR_Assignment>(n.src_loc, idx, value, Symbol_Scope::None);
        _return(assign);
    }
    else if (auto mem = dynamic_cast<IR_Member_Access*>(lval))
    {
        auto assign = ir->alloc<IR_Assignment>(n.src_loc, mem, value, Symbol_Scope::Field);
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
    if (symbol_already_declared_here(n.variable_name))
    {
        n.src_loc.report("error", "Cannot declare variable because it has already been declared");
        abort();
    }
    auto symbol = locality->current().symbols().make_symbol(n.variable_name, n.type->type, n.src_loc, cur_symbol_scope);
    assert(symbol);
    symbol->is_readonly = n.is_readonly;
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
    else if (!val_ty->is_assignable_to(n.decl->type->type))
    {
        n.src_loc.report("error", "Cannot assign type `%s' to type `%s'",
                         val_ty->name().c_str(),
                         n.decl->type->type->name().c_str());
        abort();
    }
    auto variable = get<IR_Symbol*>(*n.decl);
    assert(variable);
    variable->is_readonly = false;
    variable->is_initialized = true;
    auto assign = ir->alloc<IR_Assignment>(n.src_loc, variable, value, variable->scope);
    _return(assign);
}

void Ast_To_IR::visit(Decl_Constant_Node &n)
{
    auto variable = get<IR_Symbol*>(*n.decl);
    assert(variable);
    variable->is_initialized = true;
    variable->is_readonly = true;
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
    auto old_fn_ep = cur_fn_ep;
    auto old_returns = all_returns_this_fn;
    defer({cur_symbol_scope = old_scope;
            cur_locals_count = old_locals_count;
            cur_fn = old_fn;
            cur_fn_ep = old_fn_ep;
            all_returns_this_fn = old_returns;});
    cur_fn = &n;

    if (n.is_bound())
    {   // We need to ensure some VARIABLE does not already have this name, otherwise we don't
        // care.
        // @Design: This may be scrapped as it's worth considering allowing bound functions
        // to be defined anywhere/out-of-order in the source code.
        if (auto exists = find_symbol(n.bound_name))
        {
            n.src_loc.report("error", "Cannot declare bound function because a variable with that name already exists.");
            exists->src_loc.report("here", "");
            abort();
        }

    }

    // we need some way to store all returns created during this function definition so we can decide
    // whether or not we use the Return_Fast instruction
    std::vector<IR_Return*> returns_this_fn;
    all_returns_this_fn = &returns_this_fn;
    const bool is_void_return = n.fn_type->return_type() == ir->types->get_void();
    locality->push(true);
    auto fn_body = ir->labels->make_named_block(label_name_gen(), label_name_gen(), n.src_loc);
    assert(fn_body);
    cur_fn_ep = fn_body;
    // Since code can be free (outside of a function), we opt to define a function wherever we
    // are. We don't want that code to accidently start executing so we just create a branch
    // that jumps over the body of the function.
    auto branch_over_body = ir->alloc<IR_Branch>(n.src_loc, fn_body->end());

    cur_symbol_scope = Symbol_Scope::Local;
    auto params_copy = n.params;
    Decl_Node *self_decl = nullptr;
    if (is_extending)
    {
        if (n.is_bound())
        {
            if (auto method_exists =
                is_extending->get_method(n.bound_name, n.fn_type->parameter_types()))
            {
                if (method_exists->is_waiting_for_definition())
                {
                    method_exists->set_function(fn_body);
                }
                else
                {
                    n.src_loc.report("error", "Method `%s %s' already defined for type `%s'",
                                     n.bound_name.c_str(),
                                     n.fn_type->name().c_str(),
                                     is_extending->name().c_str());
                    abort();
                }
            }
            else
            {
                auto method = new Method_Info(n.bound_name, n.fn_type, fn_body);
                is_extending->add_method(method);
            }
            // don't insert self reference to non-bound methods.
            self_decl = new Decl_Node{n.src_loc, "self", new Type_Node{n.src_loc, is_extending}, true};
            assert(self_decl);
            params_copy.insert(params_copy.begin(), self_decl);
        }
    }

    std::vector<IR_Symbol*> arg_symbols;
    for (auto &&a : params_copy)
    {   // Declare args in order from left to right
        auto p_sym = get<IR_Symbol*>(*a);
        assert(p_sym);
        p_sym->is_initialized = true;
        arg_symbols.push_back(p_sym);
    }
    for (auto &&it = arg_symbols.rbegin(); it != arg_symbols.rend(); ++it)
    {   // Args are pushed on the stack from left to right so they need to be pulled out
        // in the reverse order they were declared.
        auto assign_arg_to_local = ir->alloc<IR_Assign_Top>((*it)->src_loc, *it, cur_symbol_scope);
        fn_body->body().push_back(assign_arg_to_local);
    }
    if (self_decl)
    {
        delete self_decl; // @XXX this is shite.
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

    // @FixMe: find a way for this to just return a block instead of directly inserting
    // like this
    ir->first.push_back(branch_over_body);
    ir->first.push_back(fn_body);

    locality->pop();

    if (n.is_bound())
    {
        if (!locality->current().bound_functions().add(n.bound_name, n.fn_type, fn_body))
        {
            n.src_loc.report("error", "Cannot bind %s to `%s' because it already exists",
                             n.fn_type->name().c_str(), n.bound_name.c_str());
            abort();
        }
    }

    auto callable = ir->alloc<IR_Callable>(n.src_loc, fn_body, n.fn_type, n.is_bound());
    _return(callable)
}

void Ast_To_IR::visit(List_Node &n)
{
    // never executed.
    NOT_IMPL;
}

void Ast_To_IR::visit(Integer_Node &n)
{
    auto fixnum = ir->alloc<IR_Fixnum>(n.src_loc, ir->types->get_int(), static_cast<Fixnum>(n.value));
    _return(fixnum);
}

void Ast_To_IR::visit(Real_Node &n)
{
    auto real = ir->alloc<IR_Double>(n.src_loc, ir->types->get_double(), n.value);
    _return(real);
}

void Ast_To_IR::visit(String_Node &n)
{
    auto string = ir->alloc<IR_String>(n.src_loc, ir->types->get_string(), strings->size());
    strings->push_back(n.value);
    _return(string);
}

void Ast_To_IR::visit(Boolean_Node &n)
{
    auto boolean = ir->alloc<IR_Boolean>(n.src_loc, ir->types->get_bool(), n.value);
    _return(boolean);
}

void Ast_To_IR::visit(Character_Node &n)
{
    auto chr = ir->alloc<IR_Fixnum>(n.src_loc, ir->types->get_char(), n.value);
    _return(chr);
}

void Ast_To_IR::visit(Logical_Or_Node &n)
{

    std::vector<IR_Node*> block;
    auto lhs = get<IR_Value*>(*n.lhs);
    assert(lhs);
    if (lhs->get_type() != ir->types->get_bool())
    {
        lhs->src_loc.report("error", "Expected type `bool' got `%s'",
                            lhs->get_type()->name().c_str());
        abort();
    }
    auto rhs = get<IR_Value*>(*n.rhs);
    assert(rhs);
    if (rhs->get_type() != ir->types->get_bool())
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

    auto r_block = ir->alloc<IR_Block>(n.src_loc, block, ir->types->get_bool());
    _return(r_block);
}

void Ast_To_IR::visit(Logical_And_Node &n)
{
    auto old_true_label = cur_true_label;
    auto true_label = ir->labels->make_label(label_name_gen(), n.lhs->src_loc);
    cur_true_label = true_label;
    std::vector<IR_Node*> block;
    auto lhs = get<IR_Value*>(*n.lhs);
    assert(lhs);
    if (lhs->get_type() != ir->types->get_bool())
    {
        lhs->src_loc.report("error", "Expected type `bool' got `%s'",
                            lhs->get_type()->name().c_str());
        abort();
    }
    cur_true_label = old_true_label;
    auto rhs = get<IR_Value*>(*n.rhs);
    assert(rhs);
    if (rhs->get_type() != ir->types->get_bool())
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

    auto r_block = ir->alloc<IR_Block>(n.src_loc, block, ir->types->get_bool());
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
    defer1(cur_call_arg_types = old_cur_call_arg_types);

    Function_Parameters fp(args_types);
    cur_call_arg_types = &fp;

    auto callee = get<IR_Value*>(*n.callee);
    assert(callee);
    if (auto alloc = dynamic_cast<IR_Allocate_Object*>(callee))
    {   // figure out which ctor to call or error...
        if (alloc->for_type == ir->types->get_buffer())
        {
            if (args_types.size() == 1
                && args_types[0] == ir->types->get_int())
            {
                alloc->args = args;
                _return(alloc);
            }
        }
        auto ctor = alloc->for_type->get_constructor(fp);
        if (!ctor)
        {
            n.src_loc.report("error", "No constructor for `%s' matches arguments `%s'",
                             alloc->for_type->name().c_str(), fp.to_string().c_str());
            abort();
        }
        alloc->args = args;
        alloc->which_ctor = ctor;
        _return(alloc);
    }
    if (auto member = dynamic_cast<IR_Member_Access*>(callee))
    {
        auto thing_ty = member->thing->get_type();
        assert(thing_ty);
        if (auto method = thing_ty->get_method(member->member_name, args_types))
        {
            auto call_meth = ir->alloc<IR_Call_Method>(n.src_loc, member->thing, method, args); 
            _return(call_meth);
        }
        else if (!thing_ty->get_field(member->member_name))
        {
            thing_ty->dump();
            n.src_loc.report("error", "Method `fn %s%s' not implemented for type `%s'",
                             member->member_name.c_str(), 
                             fp.to_string().c_str(),
                             thing_ty->name().c_str());
            abort();
        }
    }
    if (auto meth_call = dynamic_cast<IR_Call_Method*>(callee))
    {
        meth_call->arguments = args;
        _return(meth_call);
    }
    else
    {
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
    }
    auto call = ir->alloc<IR_Call>(n.src_loc, callee, args);
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
        assert(n.subscript);
        if (n.subscript->contents.size() != 1)
        {
            n.src_loc.report("error", "Buffer index only takes one argument");
            abort();
        }
        std::vector<IR_Value*> args;
        for (auto &&n : n.subscript->contents)
        {
            auto a = get<IR_Value*>(*n);
            assert(a);
            args.push_back(a);
        }
        auto indexable = ir->alloc<IR_Indexable>(n.src_loc, thing, ir->types->get_char(), args);
        _return(indexable);
    }
    else if (auto arr_ty = dynamic_cast<Array_Type_Info*>(thing_ty))
    {
        assert(n.subscript);
        if (n.subscript->contents.size() != 1)
        {
            n.src_loc.report("error", "Buffer index only takes one argument");
            abort();
        }
        std::vector<IR_Value*> args;
        for (auto &&n : n.subscript->contents)
        {
            auto a = get<IR_Value*>(*n);
            assert(a);
            args.push_back(a);
        }
        auto indexable = ir->alloc<IR_Indexable>(n.src_loc, thing, arr_ty->of_type(), args);
        _return(indexable);
    }
    else
    {
        std::vector<IR_Value*> args;
        std::vector<Type_Info*> arg_types;
        for (auto &&n : n.subscript->contents)
        {
            auto a = get<IR_Value*>(*n);
            assert(a);
            auto t = a->get_type();
            assert(t);
            args.push_back(a);
            arg_types.push_back(t);
        }
        if (auto method = thing_ty->get_method("[]", arg_types))
        {
            auto indexable = ir->alloc<IR_Indexable>(
                n.src_loc, thing, method->type()->return_type(), args);
            _return(indexable);
        }
        else
        {
            n.src_loc.report("error", "[] operator not implemented for type `%s'",
                             thing_ty->name().c_str());
            abort();
        }
    }
}

void Ast_To_IR::visit(Member_Accessor_Node &n)
{
    auto thing = get<IR_Value*>(*n.thing);
    assert(thing);
    auto member_access = ir->alloc<IR_Member_Access>(n.src_loc, thing, n.member->name());
    _return(member_access);
}

void Ast_To_IR::visit(Negate_Node &n)
{
    auto operand = get<IR_Value*>(*n.operand);
    if (!operand)
    {
        n.operand->src_loc.report("error", "Expected a value");
        abort();
    }
    auto uop = ir->alloc<IR_U_Negate>(n.src_loc, operand);
    _return(uop);
}

void Ast_To_IR::visit(Positive_Node &n)
{
    auto operand = get<IR_Value*>(*n.operand);
    if (!operand)
    {
        n.operand->src_loc.report("error", "Expected a value");
        abort();
    }
    auto uop = ir->alloc<IR_U_Positive>(n.src_loc, operand);
    _return(uop);
}

void Ast_To_IR::visit(Not_Node &n)
{
    auto operand = get<IR_Value*>(*n.operand);
    if (!operand)
    {
        n.operand->src_loc.report("error", "Expected a value");
        abort();
    }
    auto uop = ir->alloc<IR_U_Not>(n.src_loc, operand);
    _return(uop);
}

void Ast_To_IR::visit(Invert_Node &n)
{
    auto operand = get<IR_Value*>(*n.operand);
    if (!operand)
    {
        n.operand->src_loc.report("error", "Expected a value");
        abort();
    }
    auto uop = ir->alloc<IR_U_Invert>(n.src_loc, operand);
    _return(uop);
}

void Ast_To_IR::visit(Constructor_Node &n)
{

    /*
     * FixMe: Alot of this code is shared with Fn_Node and severely needs to be factored.
     */



    // Use the stack to save state becaue functions can be nested
    auto old_scope = cur_symbol_scope;
    auto old_locals_count = cur_locals_count;
    defer({cur_symbol_scope = old_scope;
            cur_locals_count = old_locals_count;});

    // we need some way to store all returns created during this function definition so we can
    // decide whether or not we use the Return_Fast instruction
    std::vector<IR_Return*> returns_this_fn;
    all_returns_this_fn = &returns_this_fn;
    const bool is_void_return = n.fn_type->return_type() == ir->types->get_void();
    assert(is_void_return); // constructor must be void return.
    assert(is_extending);
    locality->push(true);
    auto ctor_body =
        ir->labels->make_named_block(label_name_gen(), label_name_gen(), n.src_loc);
    assert(ctor_body);
    auto branch_over_body = ir->alloc<IR_Branch>(n.src_loc, ctor_body->end());

    cur_symbol_scope = Symbol_Scope::Local;
    if (auto ctor_exists =
        is_extending->get_constructor(n.fn_type->parameter_types()))
    {
        if (ctor_exists->is_waiting_for_definition())
        {
            ctor_exists->set_function(ctor_body);
        }
        else
        {
            n.src_loc.report("error", "Constructor `%s' already defined for type `%s'",
                             n.fn_type->name().c_str(),
                             is_extending->name().c_str());
            abort();
        }
    }
    else
    {
        auto ctor = new Constructor_Info(n.fn_type, ctor_body);
        is_extending->add_constructor(ctor);
    }
    auto self_decl = new Decl_Node{n.src_loc, "self", new Type_Node{n.src_loc, is_extending}, true};

    auto params_copy = n.params;
    params_copy.insert(params_copy.begin(), self_decl);
    std::vector<IR_Symbol*> arg_symbols;
    for (auto &&a : params_copy)
    {   // Declare args in order from left to right
        auto p_sym = get<IR_Symbol*>(*a);
        assert(p_sym);
        p_sym->is_initialized = true;
        arg_symbols.push_back(p_sym);
    }
    for (auto &&it = arg_symbols.rbegin(); it != arg_symbols.rend(); ++it)
    {   // Args are pushed on the stack from left to right so they need to be pulled out
        // in the reverse order they were declared.
        auto assign_arg_to_local = ir->alloc<IR_Assign_Top>((*it)->src_loc, *it, cur_symbol_scope);
        ctor_body->body().push_back(assign_arg_to_local);
    }
    if (self_decl)
    {
        delete self_decl; // @XXX this is shite.
    }
    convert_body(n.body, ctor_body->body());
    if (ctor_body->body().empty())
    {
        auto empty_retn = ir->alloc<IR_Return>(n.src_loc, std::vector<IR_Value*>(), cur_locals_count != 0);
        all_returns_this_fn->push_back(empty_retn);
        ctor_body->body().push_back(empty_retn); 
    }
    else if (auto last = ctor_body->body().back())
    {
        if (dynamic_cast<IR_Return*>(last) == nullptr)
        {
            auto last_retn = ir->alloc<IR_Return>(n.src_loc, std::vector<IR_Value*>(), cur_locals_count != 0);
            all_returns_this_fn->push_back(last_retn);
            ctor_body->body().push_back(last_retn); 
        }
    }
    if (cur_locals_count > 0)
    {
        auto num_locals_to_alloc = ir->alloc<IR_Allocate_Locals>(n.src_loc, cur_locals_count);
        ctor_body->body().insert(ctor_body->body().begin(), num_locals_to_alloc);
        for (auto &&ret : *all_returns_this_fn)
        {
            ret->should_leave = true;
        }
    }

    std::vector<IR_Node*> ctor_block;
    ctor_block.push_back(branch_over_body);
    ctor_block.push_back(ctor_body);

    locality->pop();

    auto ctor = ir->alloc<IR_Block>(n.src_loc, ctor_block, ir->types->get_void());
    _return(ctor)
}

void Ast_To_IR::visit(Type_Def_Node &n)
{
    /*
      The most complicated part of this will be generating the hidden .init method 
      to initialize default values.

      Fields:
      - declare fields for this type so they me be referenced inside of .init and 
      constructors

      .init:
      it takes 1 argument with the same type as it is being implemented for and returns
      back the default-constructed object.
        + convert pure field declarations into default values if they are pimitive 
        types: int = 0, double = 0.0, bool = false
          ? alternatively uninitialized values could be required to be Optional<T>
        + otherwise alloc + default construct them
          * it is an error to declare a non-trivial type with no default constructor
        + convert decl_assign and decl_const to their respective code + Set_Field <n>
      
      Constructors:
      - ensure each constructor's signature is unique and translate into code and 
      link to the type
      
      Methods:
      - ensure each method's signature is unique and translate into code and link 
      to the type
     */
    
    assert(n.type);
    auto old_type = is_extending;
    auto old_scope = cur_symbol_scope;
    defer({is_extending = old_type;
            cur_symbol_scope = old_scope;});

    locality->push(true);
    is_extending = n.type;


    for (auto &&method : n.methods)
    {   // Declare methods so ordering is agnostic
        assert(method->is_bound()); // parsing messed up if this fails...
        locality->current().bound_functions().declare_method(is_extending,
                                                             method->bound_name,
                                                             method->fn_type);
    }
    if (n.constructors.empty())
    {
        auto empty_void_fn = ir->types->declare_function({}, ir->types->get_void(), false);
        auto default_ctor = new Constructor_Info{empty_void_fn};
        is_extending->add_constructor(default_ctor);
    }
    else
    {
        for (auto &&ctor : n.constructors)
        {   // Declare constructors so their ordering is agnostic
            auto ctor_info = new Constructor_Info{ctor->fn_type};
            if (!is_extending->add_constructor(ctor_info))
            {
                ctor->src_loc.report("error", "Constructor `%s' already defined for type `%s'",
                                     ctor->fn_type->name().c_str(), is_extending->name().c_str());
                abort();
            }
        }
    }
    std::vector<IR_Node*> type_definition;

    cur_symbol_scope = Symbol_Scope::Field;
    auto init = ir->labels->make_named_block(label_name_gen(), label_name_gen(), n.src_loc);
    assert(init);
    auto branch_over_body = ir->alloc<IR_Branch>(n.src_loc, init->end());
    auto alloc = ir->alloc<IR_Allocate_Locals>(n.src_loc, 1);
    init->body().push_back(alloc);
    auto store_self =
        ir->alloc<IR_Assign_Top>(
            n.src_loc,
            ir->alloc<IR_Fixnum>(n.src_loc, ir->types->get_int(), 0),
            Symbol_Scope::Local);
    init->body().push_back(store_self);

    for (auto &&field : n.fields)
    {
        Field_Info *field_to_add = nullptr;
        if (auto decl_assign = dynamic_cast<Decl_Assign_Node*>(field))
        {
            auto assign = get<IR_Assignment*>(*decl_assign);
            field_to_add = new Field_Info{
                decl_assign->decl->variable_name, assign->rhs->get_type(), false};
            init->body().push_back(assign);
        }
        else if (auto decl_cons = dynamic_cast<Decl_Constant_Node*>(field))
        {
            auto assign = get<IR_Assignment*>(*decl_cons);
            field_to_add = new Field_Info{
                decl_cons->decl->variable_name, assign->rhs->get_type(), true};
            init->body().push_back(assign);
        }
        else if (dynamic_cast<Decl_Node*>(field))
        {
            n.src_loc.report("NYI", "Uninitialized declarations not implemented for type definitions");
            abort();
        }
        else
        {
            field->src_loc.report("error", "not sure how a %s ended up in the fields...",
                                 field->type_name().c_str());
            abort();
        }

        if (field_to_add && !is_extending->add_field(field_to_add))
        {
            field->src_loc.report("error", "Field `%s' already defined for `%s'",
                                  field_to_add->name().c_str(),
                                  is_extending->name().c_str());
            abort();
        }
    }
    auto retn = ir->alloc<IR_Return>(n.src_loc, std::vector<IR_Value*>(), true);
    init->body().push_back(retn);
    auto init_fn_ty = ir->types->declare_function({is_extending}, is_extending, false);
    auto init_ctor = new Constructor_Info{init_fn_ty, init};
    is_extending->init(init_ctor);
    type_definition.push_back(branch_over_body);
    type_definition.push_back(init);

    for (auto &&method : n.methods)
    {
        auto converted_method = get(*method);
        assert(converted_method);
        type_definition.push_back(converted_method);
    }


    cur_symbol_scope = Symbol_Scope::Local;
    
    for (auto &&ctor : n.constructors)
    {
        auto converted_ctor = get(*ctor);
        assert(converted_ctor);
        type_definition.push_back(converted_ctor);
    }

    locality->pop();
    auto ret = ir->alloc<IR_Block>(n.src_loc, type_definition, ir->types->get_void());
    _return(ret);
}

void Ast_To_IR::visit(Unalias_Node &n)
{
    assert(n.value);
    auto val = get<IR_Value*>(*n.value);
    assert(val);
    auto alias = val->get_type();
    auto top = alias->aliased_to_top();
    if (alias == top)
    {
        _return(val);
    }
    std::vector<IR_Node*> unalias;
    unalias.push_back(val);
    auto ret = ir->alloc<IR_Block>(n.src_loc, unalias, top);
    _return(ret);
}

void Ast_To_IR::visit(Type_Alias_Node &n)
{
    // the aliasing happens in the parse phase.
    _return(nullptr);
}

void Ast_To_IR::visit(struct Extend_Node&n)
{
    std::vector<IR_Node*> block;
    assert(n.for_type);
    assert(n.for_type->type);

    auto old_is_extending = is_extending;
    defer1(is_extending = old_is_extending);
    locality->push(true);
    is_extending = n.for_type->type;
    for (auto &&fn : n.body)
    {   // @FixMe: Need to declare these so source ordering earlier defined functions may
        // refer to later ones.
        auto converted = get(*fn);
        assert(converted);
        block.push_back(converted);
    }
    auto ret = ir->alloc<IR_Block>(n.src_loc, block, ir->types->get_void());
    locality->pop();
    _return(ret);
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
    { 
        n.src_loc.report("NYI", "multiple return values not yet implemented.");
        abort();
    }

    std::vector<IR_Value*> values;
    // 0 
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
    // 2+
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

void Ast_To_IR::visit(Break_Node &n)
{
    if (!n.values->contents.empty())
    { 
        n.src_loc.report("NYI", "multiple break values not yet implemented.");
        abort();
    }
    if (!cur_break_label)
    {
        n.src_loc.report("error", "Continue outside of a loop does not make sense.");
    }
    auto branch = ir->alloc<IR_Branch>(n.src_loc, cur_break_label);
    _return(branch);
}
                      
void Ast_To_IR::visit(Continue_Node &n)
{
    if (!cur_continue_label)
    {
        n.src_loc.report("error", "Continue outside of a loop does not make sense.");
    }
    auto branch = ir->alloc<IR_Branch>(n.src_loc, cur_continue_label);
    _return(branch);
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
        // some things like type alias and import returns nullptr.
        if (!last_node)
        {
            continue;
        }
        dst.push_back(last_node);
        if (auto val = dynamic_cast<IR_Value*>(last_node))
        {
            if (!is_last_iter || !collecting_last_node)
            {
                if (val->get_type() != ir->types->get_void())
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
    auto old_continue_label = cur_continue_label;
    auto old_break_label = cur_break_label;
    defer({cur_false_label = old_cur_false_label;
            cur_true_label = old_cur_true_label;
            cur_continue_label = old_continue_label;
            cur_break_label = old_break_label;});
    cur_false_label = loop_block->end();
    cur_true_label = loop_block;
    cur_break_label = loop_block->end();
    cur_continue_label = condition_check_label;

    assert(n.condition);
    auto cond = get<IR_Value*>(*n.condition);
    assert(cond);
    if (cond->get_type() != ir->types->get_bool())
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
    locality->push(false);
    {
        convert_body(n.body, loop_block->body());
        // The end of a while loop is always a branch back to the condition check.
        loop_block->body().push_back(ir->alloc<IR_Branch>(n.src_loc, condition_check_label));
    }
    locality->pop();

    block.push_back(loop_block);
    auto ret = ir->alloc<IR_Block>(n.src_loc, block, ir->types->get_void());
    _return(ret);
}

void Ast_To_IR::gen_for_iterator(For_Node &n, IR_Value *itr, Method_Info *move_next, Method_Info *current)
{
    auto itr_ty = itr->get_type();
    if (!move_next)
    {
        itr->src_loc.report("error", "type `%s' does no implement `fn move_next() -> bool'",
                            itr_ty->name().c_str());
        abort();
    }
    if (move_next->return_type() != ir->types->get_bool())
    {
        itr->src_loc.report("error", "`move_next' must return `bool' to qualify as an iterator",
                            itr_ty->name().c_str());
        abort();
    }
    if (!current)
    {
        itr->src_loc.report("error", "type `%s' does no implement `fn current() -> T'",
                            itr_ty->name().c_str());
        abort();
    }
    if (current->return_type() == ir->types->get_void())
    {
        itr->src_loc.report("error", "`current' must not return `void' to qualify as an iterator"); 
        abort();
    }
    // Create while loop that calls move_next() as the condition, then executes the body, then
    // declare an implicit variable called "it" that is assigned by a call to current() at
    // the start of each loop iteration.
    std::vector<IR_Node*> block;
    auto condition_check_label = ir->labels->make_label(label_name_gen(), n.src_loc);
    assert(condition_check_label);
    auto loop_block = ir->labels->make_named_block(label_name_gen(), label_name_gen(), n.src_loc);
    assert(loop_block);
    auto old_cur_false_label = cur_false_label;
    auto old_cur_true_label = cur_true_label;
    auto old_continue_label = cur_continue_label;
    auto old_break_label = cur_break_label;
    defer({cur_false_label = old_cur_false_label;
            cur_true_label = old_cur_true_label;
            cur_continue_label = old_continue_label;
            cur_break_label = old_break_label;});
    cur_false_label = loop_block->end();
    cur_true_label = loop_block;
    cur_break_label = loop_block->end();
    cur_continue_label = condition_check_label;

    // This allows us to create the iterator in the first step of the for loop and continue using it
    locality->push(false);
    auto itr_sym = locality->current().symbols().make_symbol(".itr", itr_ty, itr->src_loc, cur_symbol_scope);
    // @FIXME: this should be automatic
    cur_locals_count++;
    itr_sym->is_readonly = true;
    auto assign_itr = ir->alloc<IR_Assignment>(itr->src_loc, itr_sym, itr, cur_symbol_scope);
    block.push_back(assign_itr);
    auto condition = ir->alloc<IR_Call_Method>(itr->src_loc, itr_sym, move_next, std::vector<IR_Value*>());
    block.push_back(condition_check_label);
    block.push_back(condition);

    auto branch_if_cond_false = ir->alloc<IR_Pop_Branch_If_False>(n.src_loc, loop_block->end());
    block.push_back(branch_if_cond_false);
    {
        auto it_sym = locality->current().symbols().make_symbol(n.it, current->return_type(), itr->src_loc, cur_symbol_scope);
        cur_locals_count++;
        it_sym->is_readonly = true;
        auto call_current = ir->alloc<IR_Call_Method>(itr->src_loc, itr_sym, current, std::vector<IR_Value*>());
        auto assign_it = ir->alloc<IR_Assignment>(itr->src_loc, it_sym, call_current, cur_symbol_scope);
        loop_block->body().push_back(assign_it);
        convert_body(n.body, loop_block->body());
        // The end of a loop is always a branch back to the condition check.
        loop_block->body().push_back(ir->alloc<IR_Branch>(n.src_loc, condition_check_label));
    }
    locality->pop();

    block.push_back(loop_block);
    auto ret = ir->alloc<IR_Block>(n.src_loc, block, ir->types->get_void());
    _return(ret);
}

void Ast_To_IR::visit(For_Node &n)
{
    assert(n.iterable);
    auto itr = get<IR_Value*>(*n.iterable);
    assert(itr);
    auto itr_ty = itr->get_type();
    assert(itr_ty);

    // Something "iterable" is an array, a buffer, or has both of:
    //    fn move_next() -> bool
    //    fn current() -> T
    if (itr_ty == ir->types->get_buffer())
    {
        itr->src_loc.report("NYI", "iterators not yet implemented for buffers");
    }
    else if (dynamic_cast<Array_Type_Info*>(itr_ty))
    {
        itr->src_loc.report("NYI", "iterators not yet implemented for arrays");
    }
    else
    {
        auto move_next = itr_ty->get_method("move_next", {});
        auto current = itr_ty->get_method("current", {});
        gen_for_iterator(n, itr, move_next, current);
    }
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
    defer({cur_false_label = old_cur_false_label;
            cur_true_label = old_cur_true_label;});
    auto alt_begin_label = ir->labels->make_label(label_name_gen(), n.src_loc);
    auto cons_begin_label = ir->labels->make_label(label_name_gen(), n.src_loc);
    assert(alt_begin_label);
    cur_false_label = alt_begin_label;
    cur_true_label = cons_begin_label;
    std::vector<IR_Node*> block;
    assert(n.condition);
    auto cond = get<IR_Value*>(*n.condition);
    assert(cond);
    if (cond->get_type() != ir->types->get_bool())
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
    locality->push(false);
    convert_body(n.consequence, block, (is_either_empty ? nullptr : &last_conseq));
    locality->pop();
    if (!n.alternative.empty())
    {
        auto end_label = ir->labels->make_label(label_name_gen(), n.src_loc);
        auto branch_to_end = ir->alloc<IR_Branch>(n.src_loc, end_label);
        block.push_back(branch_to_end);
        block.push_back(alt_begin_label);
        locality->push(false);
        convert_body(n.alternative, block, (is_either_empty ? nullptr : &last_altern));
        locality->pop();
        block.push_back(end_label);
    }
    else
    {
        block.push_back(alt_begin_label);
    }

    auto if_else_type = deduce_type(ir->types->get_void(), last_conseq, last_altern);
    auto ret = ir->alloc<IR_Block>(n.src_loc, block, if_else_type);
    _return(ret);
}

void Ast_To_IR::visit(struct Array_Literal_Node &n)
{
    assert(n.values);
    std::vector<IR_Value*> values;
    Type_Info *of_type = nullptr;
    for (auto c : n.values->contents)
    {
        auto v = get<IR_Value*>(*c);
        if (!v)
        {
            c->src_loc.report("error", "Expected a value.");
            abort();
        }
        auto v_ty = v->get_type();
        if (!of_type)
        {
            of_type = v_ty;
            assert(of_type);
        }
        else if (of_type != v_ty)
        {
            c->src_loc.report("error",
                              "Union types not yet implemented and the expected type of this array literal is `%s', got a `%s'",
                              of_type->name().c_str(), v_ty->name().c_str());
            abort();
        }
        values.push_back(v);
    }
    if (values.empty())
    {
        n.src_loc.report("error", "Cannot deduce type for an empty array literal!");
        abort();
    }

    std::vector<IR_Node*> block;
    locality->push(false);
    auto array_type = ir->types->get_array_type(of_type);
    auto size = ir->alloc<IR_Fixnum>(n.src_loc, ir->types->get_int(), values.size());
    auto new_array = ir->alloc<IR_New_Array>(n.src_loc, array_type, of_type, size);
    auto arr_tmp = locality->current().symbols().make_symbol(".array_lit",
                                                             array_type,
                                                             n.src_loc,
                                                             cur_symbol_scope);
    arr_tmp->is_readonly = true;
    auto assign_arr = ir->alloc<IR_Assignment>(n.src_loc, arr_tmp, new_array, cur_symbol_scope);

    // Create the array
    block.push_back(assign_arr);
    // Assign the values
    for (Fixnum i = 0; i < (Fixnum)values.size(); ++i)
    {
        auto idx = ir->alloc<IR_Fixnum>(n.src_loc, ir->types->get_int(), i);
        auto val = values[i];
        std::vector<IR_Value*> arg{idx};
        auto indexable = ir->alloc<IR_Indexable>(n.src_loc, arr_tmp, of_type, arg);
        auto assign_to_idx = ir->alloc<IR_Assignment>(n.src_loc, indexable, val, cur_symbol_scope);
        block.push_back(assign_to_idx);
    }
    // And finally give the array back
    block.push_back(arr_tmp);
    locality->pop();
    auto ret = ir->alloc<IR_Block>(n.src_loc, block, array_type);
    _return(ret);
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
    if (!size_ty->is_assignable_to(ir->types->get_int()))
    {
        size->src_loc.report("error", "Array size must be an integer type, got `%s'",
                             size_ty->name().c_str());
        abort();
    }
    auto new_array = ir->alloc<IR_New_Array>(n.src_loc, n.array_type, n.of_type->type, size);
    _return(new_array);
}

void Ast_To_IR::convert(Ast &ast, Malang_IR *ir, Module_Map *mod_map, Scope_Lookup *global, std::vector<String_Constant> *strings)
{
    assert(ir);
    assert(mod_map);
    assert(global);
    assert(strings);

    cur_call_arg_types = nullptr;
    cur_symbol_scope = Symbol_Scope::Global;
    cur_locals_count = 0;
    cur_module = nullptr;
    cur_fn = nullptr;
    is_extending = nullptr;
    cur_fn_ep = nullptr;
    cur_true_label = cur_false_label = nullptr;
    cur_continue_label = cur_break_label = nullptr;
    this->mod_map = mod_map;
    this->ir = ir;
    this->locality = global;
    this->globals = global;
    this->strings = strings;
    convert_intern(ast);
}
void Ast_To_IR::convert_intern(Ast &ast)
{
    for (auto imp : ast.imports)
    {
        get(*imp);
    }
    // @Design: Maybe declare everything in first, then define everything in second and then
    // come back and define everything in first. This allows global variables to be referenced
    // in named functions. It also allows free-code to reference any types, extensions, and
    // named functions

    // Extensions, Type definitions, Named functions in their source order
    convert_body(ast.first, ir->first);
    // Free statements and expressions.
    convert_body(ast.second, ir->second);
}

IR_Symbol *Ast_To_IR::find_symbol(const std::string &name)
{
    return locality->current().find_symbol(name);
}

bool Ast_To_IR::symbol_already_declared_here(const std::string &name)
{
    return locality->current().symbols().any(name);
}
