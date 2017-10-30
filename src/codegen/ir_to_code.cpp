#include "ir_to_code.hpp"
#include "../ir/nodes.hpp"
#include "../vm/runtime/reflection.hpp"
#include "../vm/runtime/primitive_types.hpp"
#include "codegen.hpp"

#define NOT_IMPL {printf("IR_To_Code visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

void IR_To_Code::visit(IR_Noop &n)
{
    cg->push_back_noop();
}

void IR_To_Code::visit(IR_Discard_Result &n)
{
    if (!skip_next_drop)
        cg->push_back_drop(n.num);
    skip_next_drop = false;
}

void IR_To_Code::visit(IR_Block &n)
{
    convert_many(n.nodes);
}

void IR_To_Code::visit(IR_Boolean &n)
{
    cg->push_back_literal_32(n.value);
}

void IR_To_Code::visit(IR_Char &n)
{
    cg->push_back_literal_value(n.value);
}

void IR_To_Code::visit(IR_Fixnum &n)
{
    cg->push_back_literal_32(n.value);
}

void IR_To_Code::visit(IR_Single &n)
{
    cg->push_back_literal_value(n.value);
}

void IR_To_Code::visit(IR_Double &n)
{
    cg->push_back_literal_double(n.value);
}

void IR_To_Code::visit(IR_New_Array &n)
{
    convert_one(*n.size);
    cg->push_back_array_new(n.of_type);
}

void IR_To_Code::visit(IR_String &n)
{
    cg->push_back_load_string_constant(n.strings_index);
}

void IR_To_Code::visit(IR_Symbol &n)
{
    if (!n.is_initialized)
    {
        n.src_loc.report("error", "Use of uninitialized variable");
        abort();
    }
    switch (n.scope)
    {
        default: printf("don't know this scope!\n"); abort();
        case Symbol_Scope::Global:
            cg->push_back_load_global(n.index);
            break;
        case Symbol_Scope::Local:
            cg->push_back_load_local(n.index);
            break;
        case Symbol_Scope::Field:
            NOT_IMPL;
            break;
    }
}

void IR_To_Code::visit(struct IR_Callable &n)
{
    if (n.fn_type->is_native())
    {
        cg->push_back_literal_32(n.u.index);
    }
    else if (!n.is_special_bound)
    {
        printf("%s\n", n.fn_type->name().c_str());
        assert(n.u.label->is_resolved());
        cg->push_back_literal_32(n.u.label->address());
    }
    else
    {   // @FixMe: this is a hack to omit a Literal_32 followed by a Drop_1 in the case
        // of a function being hard-bound to a variable.
        skip_next_drop = true;
    }
}

void IR_To_Code::visit(struct IR_Indexable &n)
{
    printf("%s\n", n.thing->get_type()->name().c_str());
    convert_one(*n.thing);
    convert_one(*n.index);
    auto thing_ty = n.thing->get_type();
    if (thing_ty == ir->types->get_buffer())
    {
        cg->push_back_buffer_load();
    }
    else if (dynamic_cast<Array_Type_Info*>(thing_ty))
    {
        cg->push_back_array_load();
    }
    else
    {
        n.src_loc.report("NYI", "Call to index method not impl.");
        abort();
    }
}

void IR_To_Code::visit(struct IR_Member_Access &n)
{
    convert_one(*n.thing);
    auto thing_ty = n.thing->get_type();
    assert(thing_ty);
    if (n.member_name == "length")
    {
        if (thing_ty == ir->types->get_buffer())
        {
            cg->push_back_buffer_length();
            return;
        }
        else if (dynamic_cast<Array_Type_Info*>(thing_ty))
        {
            cg->push_back_array_length();
            return;
        }
    }
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Call &n)
{
    for (auto &&a : n.arguments)
    {
        convert_one(*a);
    }

    // Simple optimization if we know ahead of time the thing we're calling is literally
    // a callable. This is most useful for calling primitive builtin functions directly
    // otherwise we would waste time pushing a value to the stack before popping it and
    // finally calling it.
    if (auto callable = dynamic_cast<IR_Callable*>(n.callee))
    {
        if (callable->fn_type->is_native())
        {
            cg->push_back_call_primitive(callable->u.index);
        }
        else
        {
            assert(callable->u.label->is_resolved());
            cg->push_back_call_code(callable->u.label->address());
        }
    }
    else
    {
        convert_one(*n.callee);
        if (n.get_fn_type()->is_native())
        {
            cg->push_back_call_primitive_dyn();
        }
        else
        {
            cg->push_back_call_code_dyn();
        }
    }

}

void IR_To_Code::visit(IR_Call_Method &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Call_Virtual_Method &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Return &n)
{
    for (auto &&v : n.values)
    {
        convert_one(*v);
    }
    bool fast = !n.should_leave;
    cg->push_back_return(fast);
}

void IR_To_Code::visit(IR_Label &n)
{
    assert(!n.is_resolved());
    auto address = cg->code.size();
    n.address(address);
}

void IR_To_Code::visit(IR_Named_Block &n)
{
    visit(static_cast<IR_Label&>(n));
    convert_many(n.body());
    convert_one(*n.end());
}

void IR_To_Code::visit(IR_Branch &n)
{
    assert(n.destination);
    auto from = cg->code.size();
    if (n.destination->is_resolved())
    {
        cg->push_back_branch(n.destination->address() - from);
    }
    else
    {
        auto idx = cg->push_back_branch();
        n.destination->please_backfill_on_resolve_rel(cg, idx, from);
    }
}

void IR_To_Code::visit(IR_Pop_Branch_If_True &n)
{
    assert(n.destination);
    if (n.destination->is_resolved())
    {
        cg->push_back_pop_branch_if_true(n.destination->address());
    }
    else
    {
        auto from = cg->code.size();
        auto idx = cg->push_back_pop_branch_if_true();
        n.destination->please_backfill_on_resolve_rel(cg, idx, from);
    }
}

void IR_To_Code::visit(IR_Pop_Branch_If_False &n)
{
    assert(n.destination);
    if (n.destination->is_resolved())
    {
        cg->push_back_pop_branch_if_false(n.destination->address());
    }
    else
    {
        auto from = cg->code.size();
        auto idx = cg->push_back_pop_branch_if_false();
        n.destination->please_backfill_on_resolve_rel(cg, idx, from);
    }
}

void IR_To_Code::visit(IR_Branch_If_True_Or_Pop &n)
{
    assert(n.destination);
    if (n.destination->is_resolved())
    {
        cg->push_back_branch_if_true_or_pop(n.destination->address());
    }
    else
    {
        auto from = cg->code.size();
        auto idx = cg->push_back_branch_if_true_or_pop();
        n.destination->please_backfill_on_resolve_rel(cg, idx, from);
    }
}

void IR_To_Code::visit(IR_Branch_If_False_Or_Pop &n)
{
    assert(n.destination);
    if (n.destination->is_resolved())
    {
        cg->push_back_branch_if_false_or_pop(n.destination->address());
    }
    else
    {
        auto from = cg->code.size();
        auto idx = cg->push_back_branch_if_false_or_pop();
        n.destination->please_backfill_on_resolve_rel(cg, idx, from);
    }
}

void IR_To_Code::visit(IR_Assignment &n)
{
    // @TODO: how will array assignment be handled?
    auto lval = dynamic_cast<IR_LValue*>(n.lhs);
    assert(lval);
    auto lval_ty = lval->get_type();
    if (!lval_ty)
    {
        n.lhs->src_loc.report("error", "Could not deduce type.\n");
        abort();
    }
    auto rval_ty = n.rhs->get_type();
    if (!rval_ty)
    {
        n.rhs->src_loc.report("error", "Could not deduce type.\n");
        abort();
    }

    if (!rval_ty->is_assignable_to(lval_ty))
    {
        n.src_loc.report("error", "Cannot assign from type `%s' to `%s'\n",
                         rval_ty->name().c_str(), lval_ty->name().c_str());
        abort();
    }

    if (auto var = dynamic_cast<IR_Symbol*>(lval))
    {
        var->is_initialized = true;
        convert_one(*n.rhs);
        switch (var->scope)
        {
            default: printf("don't know this scope!\n"); abort();
            case Symbol_Scope::Global:
                cg->push_back_store_global(var->index);
                break;
            case Symbol_Scope::Local:
                cg->push_back_store_local(var->index);
                break;
            case Symbol_Scope::Field:
                NOT_IMPL;
                break;
        }
    }
    else if (auto idx = dynamic_cast<IR_Indexable*>(lval))
    {
        convert_one(*n.rhs);
        convert_one(*idx->thing);
        convert_one(*idx->index);
        if (lval->get_type() == ir->types->get_buffer())
        {
            cg->push_back_buffer_store();
        }
        else
        {
            cg->push_back_array_store();
        }
    }
    else
    {
        NOT_IMPL;
    }
}

void IR_To_Code::visit(IR_Assign_Top &n)
{
    // @TODO: how will array assignment be handled?
    auto lval = dynamic_cast<IR_LValue*>(n.lhs);
    assert(lval);
    auto lval_ty = lval->get_type();
    if (!lval_ty)
    {
        n.lhs->src_loc.report("error", "Could not deduce type.\n");
        abort();
    }
    if (auto var = dynamic_cast<IR_Symbol*>(lval))
    {
        var->is_initialized = true;
        switch (var->scope)
        {
            default: printf("don't know this scope!\n"); abort();
            case Symbol_Scope::Global:
                cg->push_back_store_global(var->index);
                break;
            case Symbol_Scope::Local:
                cg->push_back_store_local(var->index);
                break;
            case Symbol_Scope::Field:
                NOT_IMPL;
                break;
        }
    }
}

inline
void IR_To_Code::binary_op_helper(struct IR_Binary_Operation &bop)
{
    auto m = bop.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_one(*bop.lhs);
            convert_one(*bop.rhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_one(*bop.lhs);
            convert_one(*bop.rhs);
            auto label = m->code_function();
            if (label->is_resolved())
            {
                cg->push_back_call_code(label->address());
            }
            else
            {
                auto idx = cg->push_back_call_code();
                label->please_backfill_on_resolve(cg, idx);
            }
        }
    }
}

void IR_To_Code::visit(IR_B_Add &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_add();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Subtract &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_subtract();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Multiply &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_multiply();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Divide &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_divide();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Modulo &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_modulo();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_And &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_and();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Or &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_or();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Xor &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_xor();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Left_Shift &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_left_shift();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Right_Shift &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_right_shift();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Less_Than &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_less_than();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Less_Than_Equals &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_less_than_equals();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Greater_Than &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_greater_than();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Greater_Than_Equals &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_greater_than_equals();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_B_Equals &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_equals();
    }
    else
    {
        binary_op_helper(n);
    }
}

void IR_To_Code::visit(struct IR_B_Not_Equals &n)
{
    auto _int = ir->types->get_int();
    if (n.lhs->get_type() == _int && n.rhs->get_type() == _int)
    {
        convert_one(*n.lhs);
        convert_one(*n.rhs);
        cg->push_back_fixnum_not_equals();
    }
    else
    {
        binary_op_helper(n);
    }
}


void IR_To_Code::visit(IR_U_Not &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_U_Invert &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_U_Negate &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_U_Positive &n)
{
    NOT_IMPL;
}


void IR_To_Code::visit(IR_Allocate_Object &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Deallocate_Object &n)
{
    NOT_IMPL;
}
void IR_To_Code::visit(struct IR_Allocate_Locals &n)
{
    cg->push_back_alloc_locals(n.num_to_alloc);
}

void IR_To_Code::convert_many(const std::vector<IR_Node*> &n)
{
    for (auto &&one : n)
    {
        convert_one(*one);
    }
}

Codegen *IR_To_Code::convert(Malang_IR &ir)
{
    cg = new Codegen;
    this->ir = &ir;
    convert_many(ir.roots);
    cg->push_back_halt();
    return cg;
}

void IR_To_Code::convert_one(IR_Node &n)
{
    n.accept(*this);
}
