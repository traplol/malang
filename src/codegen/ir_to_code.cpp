#include "ir_to_code.hpp"
#include "../ir/nodes.hpp"
#include "../vm/runtime/reflection.hpp"
#include "../vm/runtime/primitive_types.hpp"
#include "codegen.hpp"

#define NOT_IMPL {printf("IR_To_Code visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

void IR_To_Code::visit(IR_Noop &)
{
    cg->push_back_noop();
}

void IR_To_Code::visit(IR_Discard_Result &n)
{
    if (!skip_next_drop)
        cg->push_back_drop(n.num);
    skip_next_drop = false;
}

void IR_To_Code::visit(IR_Duplicate_Result &)
{
    cg->push_back_dup_1();
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
            cg->push_back_load_field(n.index);
            break;
    }
}

void IR_To_Code::visit(IR_Callable &n)
{
    printf("%s\n", n.fn_type->name().c_str());
    if (n.fn_type->is_native())
    {
        cg->push_back_literal_32(n.u.index);
    }
    else if (!n.is_special_bound)
    {
        assert(n.u.label->is_resolved());
        cg->push_back_literal_32(n.u.label->address());
    }
    else
    {   // @FixMe: this is a hack to omit a Literal_32 followed by a Drop_1 in the case
        // of a function being hard-bound to a variable.
        skip_next_drop = true;
    }
}

void IR_To_Code::visit(IR_Method &n)
{
    NOT_IMPL;
}

static
void backfill_label(Codegen *cg, IR_Label *label)
{
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

void IR_To_Code::visit(IR_Indexable &n)
{
    convert_one(*n.thing);
    convert_one(*n.index);
    auto thing_ty = n.thing->get_type();
    assert(thing_ty);
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
        if (auto method = thing_ty->get_method("[]", {n.index->get_type()}))
        {
            if (method->is_native())
            {
                cg->push_back_call_native(method->native_function()->index);
            }
            else
            {
                backfill_label(cg, method->code_function());
            }
        }
        else
        {
            n.src_loc.report("error", "[] operator not implemented for type `%s'", thing_ty->name().c_str());
            abort();
        }
    }
}

void IR_To_Code::visit(IR_Member_Access &n)
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
    uint16_t field_idx;
    if (!thing_ty->get_field_index(n.member_name, field_idx))
    {
        n.src_loc.report("error", "type `%s' does not have a field named `%s'",
                         thing_ty->name().c_str(), n.member_name.c_str());
        abort();
    }
    cg->push_back_load_field(field_idx);
}

void IR_To_Code::visit(IR_Call &n)
{
    for (auto &&a : n.arguments)
    {
        convert_one(*a);
    }

    // Simple optimization if we know ahead of time the thing we're calling is literally
    // a callable. This is most useful for calling native builtin functions directly
    // otherwise we would waste time pushing a value to the stack before popping it and
    // finally calling it.
    if (auto callable = dynamic_cast<IR_Callable*>(n.callee))
    {
        if (auto method = dynamic_cast<IR_Method*>(n.callee))
        {
            convert_one(*method->thing);
        }
        if (callable->fn_type->is_native())
        {
            cg->push_back_call_native(callable->u.index);
        }
        else // @FixMe: Allow backfilling labels.
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
            cg->push_back_call_native_dyn();
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
                cg->push_back_store_field(var->index);
                break;
        }
    }
    else if (auto idx = dynamic_cast<IR_Indexable*>(lval))
    {
        convert_one(*idx->thing);
        convert_one(*idx->index);
        convert_one(*n.rhs);
        auto thing_ty = idx->thing->get_type();
        if (thing_ty == ir->types->get_buffer())
        {
            cg->push_back_buffer_store();
        }
        else if (dynamic_cast<Array_Type_Info*>(thing_ty))
        {
            cg->push_back_array_store();
        }
        else
        {
            if (auto method = thing_ty->get_method("[]=", {idx->index->get_type(), rval_ty}))
            {
                if (method->is_native())
                {
                    cg->push_back_call_native(method->native_function()->index);
                }
                else
                {
                    backfill_label(cg, method->code_function());
                }
            }
            else
            {
                n.src_loc.report("error", "[]= operator not implemented for type `%s'", thing_ty->name().c_str());
                abort();
            }
        }
    }
    else if (auto mem = dynamic_cast<IR_Member_Access*>(lval))
    {
        auto thing_ty = mem->thing->get_type();
        assert(thing_ty);
        auto field = thing_ty->get_field(mem->member_name);
        if (!field)
        {
            n.lhs->src_loc.report("error", "`%s' does not have a field named `%s'",
                                  thing_ty->name().c_str(), mem->member_name.c_str());
            abort();
        }
        if (field->is_readonly())
        {
            n.lhs->src_loc.report("error", "`%s.%s' is marked as readonly",
                                  thing_ty->name().c_str(), mem->member_name.c_str());
            abort();
        }
        convert_one(*mem->thing);
        convert_one(*n.rhs);
        cg->push_back_store_field(field->index());
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
                cg->push_back_store_field(var->index);
                break;
        }
    }
}

inline
void IR_To_Code::binary_op_helper(IR_Binary_Operation &bop)
{
    auto m = bop.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_one(*bop.lhs);
            convert_one(*bop.rhs);
            cg->push_back_call_native(*m->native_function());
        }
        else
        {
            convert_one(*bop.lhs);
            convert_one(*bop.rhs);
            backfill_label(cg, m->code_function());
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

void IR_To_Code::visit(IR_B_Not_Equals &n)
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


inline
void IR_To_Code::unary_op_helper(IR_Unary_Operation &uop)
{
    auto m = uop.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_one(*uop.operand);
            cg->push_back_call_native(*m->native_function());
        }
        else
        {
            convert_one(*uop.operand);
            backfill_label(cg, m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_U_Not &n)
{
    unary_op_helper(n);
}

void IR_To_Code::visit(IR_U_Invert &n)
{
    auto _int = ir->types->get_int();
    if (n.operand->get_type() == _int)
    {
        convert_one(*n.operand);
        cg->push_back_fixnum_invert();
    }
    else
    {
        unary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_U_Negate &n)
{
    auto _int = ir->types->get_int();
    if (n.operand->get_type() == _int)
    {
        convert_one(*n.operand);
        cg->push_back_fixnum_negate();
    }
    else
    {
        unary_op_helper(n);
    }
}

void IR_To_Code::visit(IR_U_Positive &n)
{
    auto _int = ir->types->get_int();
    if (n.operand->get_type() == _int)
    {
        convert_one(*n.operand);
        // noop
    }
    else
    {
        unary_op_helper(n);
    }
}


void IR_To_Code::visit(IR_Allocate_Object &n)
{
    assert(n.for_type);
    assert(n.for_type->init());
    assert(n.which_ctor);
    cg->push_back_alloc_object(n.for_type->type_token());
    // TOS is now an uninitialized object with however many fields we need, this being first
    // also puts it in Local_0 when it comes time to call the ctor
    // .init leaves our object reference on the stack so no need to duplicate it.
    auto init = n.for_type->init();
    if (init->is_native())
    {
        cg->push_back_call_native(*init->native_function());
    }
    else
    {
        backfill_label(cg, init->code_function());
    }
    // however the constructor does not return the object back so we much duplicate the
    // reference to it
    cg->push_back_dup_1();
    for (auto &&a : n.args)
    { // args from left to right.
        convert_one(*a);
    }
    if (n.which_ctor->is_native())
    {
        cg->push_back_call_native(*n.which_ctor->native_function());
    }
    else
    {
        backfill_label(cg, n.which_ctor->code_function());
    }
    // and now we are left with 1 object reference on the stack
}

void IR_To_Code::visit(IR_Deallocate_Object &n)
{
    NOT_IMPL;
}
void IR_To_Code::visit(IR_Allocate_Locals &n)
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
