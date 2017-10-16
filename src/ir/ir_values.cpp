#include "ir_values.hpp"
#include "ir_type.hpp"

__attribute__((noreturn))
IR_Type *IR_Value::get_type() const
{
    printf("get_type for `%s' not implemented.\n", type_name().c_str());
    abort();
}

METADATA_OVERRIDES_IMPL(IR_LValue);
bool IR_LValue::can_lvalue() const 
{
    return true;
}
bool IR_LValue::can_rvalue() const
{
    return true;
}

METADATA_OVERRIDES_IMPL(IR_RValue);
bool IR_RValue::can_lvalue() const 
{
    return false;
}
bool IR_RValue::can_rvalue() const
{
    return true;
}
