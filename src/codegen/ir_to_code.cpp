#include "ir_to_code.hpp"
#include "../ir/nodes.hpp"
#include "codegen.hpp"

#define NOT_IMPL {printf("IR_To_Code visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

static bool is_fixnum(IR_Node &n)
{
    if (n.type_id() == IR_Fixnum::_type_id())
    {
        return true;
    }
    return false;
}

void IR_To_Code::visit(IR_Boolean &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Char &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Fixnum &n)
{
    cg->push_back_literal_32(n.value);
}

void IR_To_Code::visit(IR_Single &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Double &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Array &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_String &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Callable &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Symbol &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Call &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Call_Method &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Call_Virtual_Method &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Return &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Label &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Named_Block &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Branch &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Branch_If_True &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Branch_If_False &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Assignment &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Add &n)
{
    convert_impl(cg, *n.lhs);
    convert_impl(cg, *n.rhs);
    if (n.lhs->type_id() == IR_Fixnum::_type_id()
        && n.rhs->type_id() == IR_Fixnum::_type_id())
    {
        /* Example optimization:
        auto lhs = static_cast<IR_Fixnum*>(n.lhs);
        auto rhs = static_cast<IR_Fixnum*>(n.rhs);
        cg->push_back_literal_32(lhs->value + rhs->value);
        */
        cg->push_back_fixnum_add();
    }
    else
    {
        NOT_IMPL;
    }
}

void IR_To_Code::visit(IR_B_Subtract &n)
{
    convert_impl(cg, *n.lhs);
    convert_impl(cg, *n.rhs);
    if (n.lhs->type_id() == IR_Fixnum::_type_id()
        && n.rhs->type_id() == IR_Fixnum::_type_id())
    {
        cg->push_back_fixnum_subtract();
    }
    else
    {
        NOT_IMPL;
    }
}

void IR_To_Code::visit(IR_B_Multiply &n)
{
    convert_impl(cg, *n.lhs);
    convert_impl(cg, *n.rhs);
    if (n.lhs->type_id() == IR_Fixnum::_type_id()
        && n.rhs->type_id() == IR_Fixnum::_type_id())
    {
        cg->push_back_fixnum_multiply();
    }
    else
    {
        NOT_IMPL;
    }
}

void IR_To_Code::visit(IR_B_Divide &n)
{
    convert_impl(cg, *n.lhs);
    convert_impl(cg, *n.rhs);
    if (n.lhs->type_id() == IR_Fixnum::_type_id()
        && n.rhs->type_id() == IR_Fixnum::_type_id())
    {
        cg->push_back_fixnum_multiply();
    }
    else
    {
        NOT_IMPL;
    }
}

void IR_To_Code::visit(IR_B_Modulo &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_And &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Or &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Xor &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Left_Shift &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Right_Shift &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Less_Than &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Less_Than_Equals &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Greater_Than &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Greater_Than_Equals &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_B_Equals &n)
{
    NOT_IMPL;
}


void IR_To_Code::visit(IR_U_Not &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_U_Invert &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_U_Negate &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_U_Positive &n)
{
    NOT_IMPL;
}


void IR_To_Code::visit(IR_Type &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Allocate_Object &n)
{
    NOT_IMPL;
}

void IR_To_Code::visit(IR_Deallocate_Object &n)
{
    NOT_IMPL;
}


Codegen *IR_To_Code::convert(Malang_IR &ir)
{
    auto cg = new Codegen;
    for (auto &&n : ir.roots)
    {
        convert_impl(cg, *n);
    }
    return cg;
}

void IR_To_Code::convert_impl(Codegen *cg, IR_Node &n)
{
    this->cg = cg;
    n.accept(*this);
}
