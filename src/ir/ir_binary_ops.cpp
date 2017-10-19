#include <cassert>
#include "ir_binary_ops.hpp"
#include "../vm/runtime/reflection.hpp"

IR_NODE_OVERRIDES_IMPL(IR_B_Add);
IR_NODE_OVERRIDES_IMPL(IR_B_Subtract);
IR_NODE_OVERRIDES_IMPL(IR_B_Multiply);
IR_NODE_OVERRIDES_IMPL(IR_B_Divide);
IR_NODE_OVERRIDES_IMPL(IR_B_Modulo);
IR_NODE_OVERRIDES_IMPL(IR_B_And);
IR_NODE_OVERRIDES_IMPL(IR_B_Or);
IR_NODE_OVERRIDES_IMPL(IR_B_Xor);
IR_NODE_OVERRIDES_IMPL(IR_B_Left_Shift);
IR_NODE_OVERRIDES_IMPL(IR_B_Right_Shift);
IR_NODE_OVERRIDES_IMPL(IR_B_Less_Than);
IR_NODE_OVERRIDES_IMPL(IR_B_Less_Than_Equals);
IR_NODE_OVERRIDES_IMPL(IR_B_Greater_Than);
IR_NODE_OVERRIDES_IMPL(IR_B_Greater_Than_Equals);
IR_NODE_OVERRIDES_IMPL(IR_B_Equals);
IR_NODE_OVERRIDES_IMPL(IR_B_Not_Equals);

Type_Info *IR_Binary_Operation::get_type() const
{
    auto m = get_method_to_call();
    assert(m);
    return m->type()->return_type();
}


static inline
Method_Info *get_bin_method(const std::string &name, IR_Value *lhs, IR_Value *rhs)
{
    assert(!name.empty());
    assert(lhs);
    assert(rhs);
    auto lhs_type = lhs->get_type();
    if (!lhs_type)
    {
        lhs->src_loc.report("error", "Could not deduce type!");
        abort();
    }
    auto rhs_type = rhs->get_type();
    if (!rhs_type)
    {
        rhs->src_loc.report("error", "Could not deduce type!");
        abort();
    }
    return lhs_type->get_method(name, {rhs_type});
}

struct Method_Info *IR_B_Add::get_method_to_call() const
{
    return get_bin_method("b_op +", lhs, rhs);
}
struct Method_Info *IR_B_Subtract::get_method_to_call() const
{
    return get_bin_method("b_op -", lhs, rhs);
}
struct Method_Info *IR_B_Multiply::get_method_to_call() const
{
    return get_bin_method("b_op *", lhs, rhs);
}
struct Method_Info *IR_B_Divide::get_method_to_call() const
{
    return get_bin_method("b_op /", lhs, rhs);
}
struct Method_Info *IR_B_Modulo::get_method_to_call() const
{
    return get_bin_method("b_op %", lhs, rhs);
}
struct Method_Info *IR_B_And::get_method_to_call() const
{
    return get_bin_method("b_op &", lhs, rhs);
}
struct Method_Info *IR_B_Or::get_method_to_call() const
{
    return get_bin_method("b_op |", lhs, rhs);
}
struct Method_Info *IR_B_Xor::get_method_to_call() const
{
    return get_bin_method("b_op ^", lhs, rhs);
}
struct Method_Info *IR_B_Left_Shift::get_method_to_call() const
{
    return get_bin_method("b_op <<", lhs, rhs);
}
struct Method_Info *IR_B_Right_Shift::get_method_to_call() const
{
    return get_bin_method("b_op >>", lhs, rhs);
}
struct Method_Info *IR_B_Less_Than::get_method_to_call() const
{
    // @TODO: require b_op < to return bool
    return get_bin_method("b_op <", lhs, rhs);
}
struct Method_Info *IR_B_Less_Than_Equals::get_method_to_call() const
{
    // @TODO: require b_op <= to return bool
    return get_bin_method("b_op <=", lhs, rhs);
}
struct Method_Info *IR_B_Greater_Than::get_method_to_call() const
{
    // @TODO: require b_op > to return bool
    return get_bin_method("b_op >", lhs, rhs);
}
struct Method_Info *IR_B_Greater_Than_Equals::get_method_to_call() const
{
    // @TODO: require b_op >= to return bool
    return get_bin_method("b_op >=", lhs, rhs);
}
struct Method_Info *IR_B_Equals::get_method_to_call() const
{
    return get_bin_method("b_op ==", lhs, rhs);
}
struct Method_Info *IR_B_Not_Equals::get_method_to_call() const
{
    return get_bin_method("b_op !=", lhs, rhs);
}
