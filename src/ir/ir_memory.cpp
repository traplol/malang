#include "ir_memory.hpp"
#include "ir_values.hpp"

IR_NODE_OVERRIDES_IMPL(IR_Allocate_Object);
IR_NODE_OVERRIDES_IMPL(IR_Deallocate_Object);

IR_Allocate_Object::~IR_Allocate_Object()
{
    arguments.clear();
}

IR_Deallocate_Object::~IR_Deallocate_Object()
{
    thing_to_deallocate = nullptr;
}
