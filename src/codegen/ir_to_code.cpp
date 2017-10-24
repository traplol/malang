#include "ir_to_code.hpp"
#include "../ir/nodes.hpp"
#include "../vm/runtime/reflection.hpp"
#include "../vm/runtime/primitive_types.hpp"
#include "codegen.hpp"

#define NOT_IMPL {printf("IR_To_Code visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

void IR_To_Code::visit(IR_Boolean &n)
{
    cg->push_back_literal_value(n.value);
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
    cg->push_back_literal_value(n.value);
}

void IR_To_Code::visit(IR_Array &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_String &n)
{
    NOT_IMPL;
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
        case Symbol_Scope::Argument:
            cg->push_back_load_arg(n.index);
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
    else
    {
        assert(n.u.label->is_resolved());
        cg->push_back_literal_32(n.u.label->address());
    }
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
            if (auto_call_cleanup)
            {
                cg->push_back_drop(n.arguments.size());
            }
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
            if (auto_call_cleanup)
            {
                cg->push_back_drop(n.arguments.size());
            }
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
    cg->push_back_return();
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
    convert_many(n.body(), true);
    convert_one(*n.end());
}

void IR_To_Code::visit(IR_Branch &n)
{
    assert(n.destination);
    if (n.destination->is_resolved())
    {
        cg->push_back_branch(n.destination->address());
    }
    else
    {
        auto from = cg->code.size();
        auto idx = cg->push_back_branch();
        n.destination->please_backfill_on_resolve_rel(cg, idx, from);
    }
}

void IR_To_Code::visit(IR_Branch_If_True &n)
{
    assert(n.destination);
    // @FixMe: does this need to generate a check against the "true" instance?
    if (n.destination->is_resolved())
    {
        cg->push_back_branch_if_not_zero(n.destination->address());
    }
    else
    {
        auto from = cg->code.size();
        auto idx = cg->push_back_branch();
        n.destination->please_backfill_on_resolve_rel(cg, idx, from);
    }
}

void IR_To_Code::visit(IR_Branch_If_False &n)
{
    NOT_IMPL;
    assert(n.destination);
    // @FixMe: does this need to generate a check against the "false" instance?
    if (n.destination->is_resolved())
    {
        cg->push_back_branch_if_zero(n.destination->address());
    }
    else
    {
        auto from = cg->code.size();
        auto idx = cg->push_back_branch();
        n.destination->please_backfill_on_resolve_rel(cg, idx, from);
    }
}

void IR_To_Code::visit(IR_Assignment &n)
{
    auto old_auto_call_cleanup = auto_call_cleanup;
    auto_call_cleanup = false;
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

    auto var = dynamic_cast<IR_Symbol*>(lval);
    if (var)
    {
        var->is_initialized = true;
        convert_one(*n.rhs);
        switch (var->scope)
        {
            default: printf("don't know this scope!\n"); abort();
            case Symbol_Scope::Global:
                cg->push_back_store_global(var->index);
                break;
            case Symbol_Scope::Argument:
                cg->push_back_store_arg(var->index);
                break;
            case Symbol_Scope::Local:
                cg->push_back_store_local(var->index);
                break;
            case Symbol_Scope::Field:
                NOT_IMPL;
                break;
        }
    }

    if (auto call = dynamic_cast<IR_Call*>(n.rhs))
    {   // Assignment from a call, we need to cleanup arguments passed.
        // Native functions maniplate the stack directly and don't need their arguments
        // dropped
        if (!call->get_fn_type()->is_native())
        {
            cg->push_back_drop(call->arguments.size());
        }
    }
    auto_call_cleanup = old_auto_call_cleanup;
}

inline
void IR_To_Code::binary_op_helper(struct IR_Binary_Operation &bop)
{
    auto m = bop.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_one(*bop.rhs);
            convert_one(*bop.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_one(*bop.rhs);
            convert_one(*bop.lhs);
            cg->push_back_call_code(m->code_function());
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

void IR_To_Code::convert_many(const std::vector<IR_Node*> &n, bool drop_unused)
{
    for (auto &&one : n)
    {
        convert_one(*one);
        if (drop_unused)
        {
            // @XXX: this is a hack out of sheer laziness and probably needs handling on a
            // case by case basis.
            auto expression = dynamic_cast<struct IR_Value*>(one);
            if (expression && expression->get_type() != ir->types->get_void())
            {   // any tree that results in a value dangling on the stack needs to be pruned
                cg->push_back_drop(1);
            }
        }
    }
}

Codegen *IR_To_Code::convert(Malang_IR &ir)
{
    cg = new Codegen;
    this->ir = &ir;
    auto_call_cleanup = true;
    convert_many(ir.roots, true);
    cg->push_back_halt();
    return cg;
}

void IR_To_Code::convert_one(IR_Node &n)
{
    n.accept(*this);
}
