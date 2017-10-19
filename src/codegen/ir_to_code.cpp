#include "ir_to_code.hpp"
#include "../ir/nodes.hpp"
#include "../vm/runtime/reflection.hpp"
#include "../vm/runtime/primitive_types.hpp"
#include "codegen.hpp"

#define NOT_IMPL {printf("IR_To_Code visitor for %s not implemented: %s:%d\n", n.type_name().c_str(), __FILE__, __LINE__); abort();}

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
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Subtract &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Multiply &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Divide &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Modulo &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_And &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Or &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Xor &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Left_Shift &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Right_Shift &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Less_Than &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Less_Than_Equals &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Greater_Than &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Greater_Than_Equals &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}

void IR_To_Code::visit(IR_B_Equals &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
}
void IR_To_Code::visit(struct IR_B_Not_Equals &n)
{
    // @TODO: use Fixnum_instruction if possible
    auto m = n.get_method_to_call();
    assert(m);
    {
        if (m->is_native())
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_primitive(*m->primitive_function());
        }
        else
        {
            convert_impl(cg, *n.rhs);
            convert_impl(cg, *n.lhs);
            cg->push_back_call_code(m->code_function());
        }
    }
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
