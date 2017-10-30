#include "ir_unary_ops.hpp"

IR_NODE_OVERRIDES_IMPL(IR_U_Not);
IR_NODE_OVERRIDES_IMPL(IR_U_Invert);
IR_NODE_OVERRIDES_IMPL(IR_U_Negate);
IR_NODE_OVERRIDES_IMPL(IR_U_Positive);

static inline
Method_Info *get_unary_method(const std::string &name, const Source_Location &src_loc, IR_Value *operand)
{
    assert(!name.empty());
    assert(operand);
    auto operand_type = operand->get_type();
    if (!operand_type)
    {
        operand->src_loc.report("error", "Could not deduce type!");
        abort();
    }
    auto method = operand_type->get_method(name, {});
    if (!method)
    {
        src_loc.report("error", "No unary operator method `%s' for type `%s' exists",
                       name.c_str(), operand_type->name().c_str(), operand_type->name().c_str());
        abort();
    }
    return method;
}

IR_Unary_Operation::~IR_Unary_Operation()
{
}
Type_Info *IR_Unary_Operation::get_type() const
{
    auto m = get_method_to_call();
    assert(m);
    return m->type()->return_type();
}

Method_Info *IR_U_Not::get_method_to_call() const
{
    return get_unary_method("!", src_loc, operand);
}
Method_Info *IR_U_Invert::get_method_to_call() const
{
    return get_unary_method("~", src_loc, operand);
}
Method_Info *IR_U_Negate::get_method_to_call() const
{
    return get_unary_method("-@", src_loc, operand);
}
Method_Info *IR_U_Positive::get_method_to_call() const
{
    return get_unary_method("+@", src_loc, operand);
}
