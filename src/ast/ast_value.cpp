#include "ast_value.hpp"

Type_Info *Ast_Value::get_type()
{
    return nullptr;
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
