#include "ir_call.hpp"

IR_NODE_OVERRIDES_IMPL(IR_Call);
IR_NODE_OVERRIDES_IMPL(IR_Call_Method);
IR_NODE_OVERRIDES_IMPL(IR_Call_Virtual_Method);
IR_NODE_OVERRIDES_IMPL(IR_Allocate_Locals);
IR_NODE_OVERRIDES_IMPL(IR_Allocate_Object);
IR_NODE_OVERRIDES_IMPL(IR_Method);
IR_NODE_OVERRIDES_IMPL(IR_Indexable);
IR_NODE_OVERRIDES_IMPL(IR_Callable);

struct Type_Info *IR_Call::get_type() const
{
    auto fn_ty = get_fn_type();
    return fn_ty->return_type();
}

struct Function_Type_Info *IR_Call::get_fn_type() const
{
    assert(callee);
    auto callee_ty = callee->get_type();
    auto fn_ty = dynamic_cast<Function_Type_Info*>(callee_ty);
    if (!fn_ty)
    {
        src_loc.report("error", "Attempting to call non function type `%s'",
                       callee_ty->name().c_str());
        abort();
    }
    return fn_ty;
}

struct Type_Info *IR_Call_Method::get_type() const
{
    return method->type()->return_type();
}

Function_Type_Info *IR_Call_Method::get_fn_type() const
{
    return method->type();
}
