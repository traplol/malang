#include "ir_unary_ops.hpp"

IR_NODE_OVERRIDES_IMPL(IR_U_Not);
IR_NODE_OVERRIDES_IMPL(IR_U_Invert);
IR_NODE_OVERRIDES_IMPL(IR_U_Negate);
IR_NODE_OVERRIDES_IMPL(IR_U_Positive);

Unary_Operation::~Unary_Operation()
{
}
