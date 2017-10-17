#include "ast_value.hpp"

__attribute__((noreturn))
Type_Info *Ast_Value::get_type()
{
    printf("get_type for `%s' not implemented.\n", type_name().c_str());
    abort();
}

bool Ast_LValue::can_lvalue() const
{
    return true;
}
bool Ast_LValue::can_rvalue() const
{
    return true;
}

bool Ast_RValue::can_lvalue() const
{
    return false;
}
bool Ast_RValue::can_rvalue() const
{
    return true;
}
