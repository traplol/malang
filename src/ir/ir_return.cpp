#include "ir_return.hpp"
#include "ir_values.hpp"

IR_NODE_OVERRIDES_IMPL(IR_Return);
IR_Return::~IR_Return()
{
    values.clear();
}
