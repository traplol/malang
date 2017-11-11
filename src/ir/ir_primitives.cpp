#include "ir_primitives.hpp"
#include "ir_label.hpp"

IR_NODE_OVERRIDES_IMPL(IR_Boolean);
IR_Boolean::~IR_Boolean()
{
}
IR_NODE_OVERRIDES_IMPL(IR_Fixnum);
IR_Fixnum::~IR_Fixnum()
{
}
IR_NODE_OVERRIDES_IMPL(IR_Single);
IR_Single::~IR_Single()
{
}
IR_NODE_OVERRIDES_IMPL(IR_Double);
IR_Double::~IR_Double()
{
}
IR_NODE_OVERRIDES_IMPL(IR_New_Array);
IR_New_Array::~IR_New_Array()
{
    size = nullptr;
}
IR_NODE_OVERRIDES_IMPL(IR_String);
IR_String::~IR_String()
{
}
