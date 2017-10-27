#include "ir_branch.hpp"
#include "ir_label.hpp"

IR_NODE_OVERRIDES_IMPL(IR_Branch);
IR_NODE_OVERRIDES_IMPL(IR_Branch_If_True);
IR_NODE_OVERRIDES_IMPL(IR_Branch_If_False);

IR_Branch::~IR_Branch()
{
    destination = nullptr;
}
