#include <cassert>
#include "ast_pretty_printer.hpp"
#include "../ast/nodes.hpp"


std::string Ast_Pretty_Printer::to_string(Ast_Node &n)
{
    n.accept(*this);
    return str.str();
}
void Ast_Pretty_Printer::reset()
{
    indent_level = 0;
    str.str(std::string());
    str.clear();
}
void Ast_Pretty_Printer::indent()
{
    indent_level++;
}

void Ast_Pretty_Printer::dedent()
{
    assert(indent_level > 0);
    indent_level--;
}
void Ast_Pretty_Printer::do_indention()
{
    for (int i = 0; i < indent_level; ++i)
    {
        str << "    ";
    }
}
void Ast_Pretty_Printer::do_body(const std::vector<Ast_Node*> &body)
{
    // I don't think this will work for nested bodies?
    str << " {\n";
    indent();
    for (auto &&n : body)
    {
        assert(n);
        do_indention();
        to_string(*n);
        str << "\n";
    }
    dedent();
    do_indention();
    str << "}";
}

void Ast_Pretty_Printer::visit(Variable_Node &n)
{
    str << n.name;
}
void Ast_Pretty_Printer::visit(Assign_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    to_string(*n.lhs);
    str << " = ";
    to_string(*n.rhs);
}
void Ast_Pretty_Printer::visit(Decl_Node &n)
{
    str << n.variable_name << " :";
    if (n.type != nullptr)
    {
        str << " " << n.type->type->name();
    }
}
void Ast_Pretty_Printer::visit(Fn_Node &n)
{
    str << "fn (";
    for (size_t i = 0; i < n.params.size(); ++i)
    {
        assert(n.params[i]);
        to_string(*n.params[i]);
        if (i + 1 < n.params.size())
        {
            str << ", ";
        }
    }
    str << ") -> ";
    assert(n.return_type);
    to_string(*n.return_type);
    do_body(n.body);
}
void Ast_Pretty_Printer::visit(List_Node &n)
{
    for (size_t i = 0; i < n.contents.size(); ++i)
    {
        assert(n.contents[i]);
        to_string(*n.contents[i]);
        if (i + 1 < n.contents.size())
        {
            str << ", ";
        }
    }
}
void Ast_Pretty_Printer::visit(Integer_Node &n)
{
    str << n.value;
}
void Ast_Pretty_Printer::visit(Real_Node &n)
{
    str << n.value;
}
void Ast_Pretty_Printer::visit(String_Node &n)
{
    str << "\"" << n.value << "\"";
}
void Ast_Pretty_Printer::visit(Boolean_Node &n)
{
    str << (n.value ? "true" : "false");
}
#define BIN_OP_PP(n, opStr) \
    str << "("; to_string(*(n).lhs); str << " " opStr " "; to_string(*(n).rhs); str << ")"

void Ast_Pretty_Printer::visit(Logical_Or_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "||");
}
void Ast_Pretty_Printer::visit(Logical_And_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "&&");
}
void Ast_Pretty_Printer::visit(Inclusive_Or_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "|");
}
void Ast_Pretty_Printer::visit(Exclusive_Or_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "^");
}
void Ast_Pretty_Printer::visit(And_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "&");
}
void Ast_Pretty_Printer::visit(Equals_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "==");
}
void Ast_Pretty_Printer::visit(Not_Equals_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "!=");
}
void Ast_Pretty_Printer::visit(Less_Than_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "<");
}
void Ast_Pretty_Printer::visit(Less_Than_Equals_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "<=");
}
void Ast_Pretty_Printer::visit(Greater_Than_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, ">");
}
void Ast_Pretty_Printer::visit(Greater_Than_Equals_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, ">=");
}
void Ast_Pretty_Printer::visit(Left_Shift_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "<<");
}
void Ast_Pretty_Printer::visit(Right_Shift_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, ">>");
}
void Ast_Pretty_Printer::visit(Add_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "+");
}
void Ast_Pretty_Printer::visit(Subtract_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "-");
}
void Ast_Pretty_Printer::visit(Multiply_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "*");
}
void Ast_Pretty_Printer::visit(Divide_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "/");
}
void Ast_Pretty_Printer::visit(Modulo_Node &n)
{
    assert(n.lhs);
    assert(n.rhs);
    BIN_OP_PP(n, "%");
}
void Ast_Pretty_Printer::visit(Call_Node &n)
{
    assert(n.callee);
    assert(n.args);
    to_string(*n.callee); str << "("; to_string(*n.args); str << ")";
}
void Ast_Pretty_Printer::visit(Index_Node &n)
{
    assert(n.thing);
    assert(n.subscript);
    to_string(*n.thing); str << "["; to_string(*n.subscript); str << "]";
}
void Ast_Pretty_Printer::visit(Field_Accessor_Node &n)
{
    assert(n.thing);
    assert(n.member);
    to_string(*n.thing); str << "."; to_string(*n.member);
}
void Ast_Pretty_Printer::visit(Negate_Node &n)
{
    assert(n.operand);
    str << "(-"; to_string(*n.operand); str << ")";
}
void Ast_Pretty_Printer::visit(Positive_Node &n)
{
    assert(n.operand);
    str << "(+"; to_string(*n.operand); str << ")";
}
void Ast_Pretty_Printer::visit(Not_Node &n)
{
    assert(n.operand);
    str << "(!"; to_string(*n.operand); str << ")";
}
void Ast_Pretty_Printer::visit(Invert_Node &n)
{
    assert(n.operand);
    str << "(~"; to_string(*n.operand); str << ")";
}
void Ast_Pretty_Printer::visit(Class_Def_Node &n)
{
    assert(n.type_info);
    str << "class " << n.type_info->name();
    if (n.has_explicit_supertype)
    {
        str << " : " << n.type_info->get_parent()->name();
    }
    str << " {\n";
    indent();
    for (auto &&f : n.fields)
    {
        do_indention();
        assert(f);
        to_string(*f);
        str << "\n";
    }
    for (auto &&c : n.constructors)
    {
        do_indention();
        assert(c);
        to_string(*c);
        str << "\n";
    }
    for (auto &&m : n.methods)
    {
        do_indention();
        assert(m);
        to_string(*m);
        str << "\n";
    }
    dedent();
    str << "}";
}
void Ast_Pretty_Printer::visit(Type_Node &n)
{
    if (n.type)
    {
        str << n.type->name();
    }
}
void Ast_Pretty_Printer::visit(Decl_Assign_Node &n)
{
    assert(n.decl);
    assert(n.value);
    to_string(*n.decl);
    str << " = ";
    to_string(*n.value);
}
void Ast_Pretty_Printer::visit(Decl_Constant_Node &n)
{
    assert(n.decl);
    assert(n.value);
    to_string(*n.decl);
    str << " : ";
    to_string(*n.value);
}
void Ast_Pretty_Printer::visit(struct Return_Node &n)
{
    str << "return";
    if (!n.values->contents.empty())
    {
        str << " ";
        to_string(*n.values);
    }
}
void Ast_Pretty_Printer::visit(struct While_Node &n)
{
    assert(n.condition);
    do_indention();
    str << "while "; to_string(*n.condition); do_body(n.body);
}
void Ast_Pretty_Printer::visit(struct If_Else_Node &n)
{
    assert(n.condition);
    str << "if "; to_string(*n.condition);
    do_body(n.consequence);
    If_Else_Node *elseif = nullptr;
    if (n.alternative.size() == 1 && (elseif = dynamic_cast<If_Else_Node*>(n.alternative[0])))
    {
        str << "\n";
        do_indention();
        str << "else ";
        to_string(*elseif);
    }
    else if (!n.alternative.empty())
    {
        str << "\n";
        do_indention();
        str << "else";
        do_body(n.alternative);
    }
}
