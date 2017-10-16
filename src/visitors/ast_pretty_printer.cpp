#include <cassert>
#include "ast_pretty_printer.hpp"
#include "../ast/nodes.hpp"


std::string Ast_Pretty_Printer::to_string(Ast_Node &n)
{
    indent_level = 0;
    str.str(std::string());
    str.clear();
    n.accept(*this);
    return str.str();
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
    str << " {" << std::endl;
    indent();
    Ast_Pretty_Printer pp;
    for (auto &&n : body)
    {
        do_indention();
        str << pp.to_string(*n) << std::endl;
    }
    dedent();
    str << "}";
}

void Ast_Pretty_Printer::visit(Variable_Node &n)
{
    str << n.name;
}
void Ast_Pretty_Printer::visit(Assign_Node &n)
{
    Ast_Pretty_Printer pp;
    str << pp.to_string(*n.lhs);
    str << " = ";
    str << pp.to_string(*n.rhs);
}
void Ast_Pretty_Printer::visit(Decl_Node &n)
{
    str << n.variable_name << " :";
    if (n.type.size() != 0)
    {
        str << " " << n.type;
    }
}
void Ast_Pretty_Printer::visit(Fn_Node &n)
{
    str << "fn (";
    Ast_Pretty_Printer pp;
    for (size_t i = 0; i < n.params.size(); ++i)
    {
        str << pp.to_string(*n.params[i]);
        if (i + 1 < n.params.size())
        {
            str << ", ";
        }
    }
    str << ") -> " << pp.to_string(*n.return_type);
    do_body(n.body);
}
void Ast_Pretty_Printer::visit(List_Node &n)
{
    Ast_Pretty_Printer pp;
    for (size_t i = 0; i < n.contents.size(); ++i)
    {
        str << pp.to_string(*n.contents[i]);
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
    Ast_Pretty_Printer pp; \
    str << "(" << pp.to_string(*(n).lhs) << " " opStr " " << pp.to_string(*(n).rhs) << ")"

void Ast_Pretty_Printer::visit(Logical_Or_Node &n)
{
    BIN_OP_PP(n, "||");
}
void Ast_Pretty_Printer::visit(Logical_And_Node &n)
{
    BIN_OP_PP(n, "&&");
}
void Ast_Pretty_Printer::visit(Inclusive_Or_Node &n)
{
    BIN_OP_PP(n, "|");
}
void Ast_Pretty_Printer::visit(Exclusive_Or_Node &n)
{
    BIN_OP_PP(n, "^");
}
void Ast_Pretty_Printer::visit(And_Node &n)
{
    BIN_OP_PP(n, "&");
}
void Ast_Pretty_Printer::visit(Equals_Node &n)
{
    BIN_OP_PP(n, "==");
}
void Ast_Pretty_Printer::visit(Not_Equals_Node &n)
{
    BIN_OP_PP(n, "!=");
}
void Ast_Pretty_Printer::visit(Less_Than_Node &n)
{
    BIN_OP_PP(n, "<");
}
void Ast_Pretty_Printer::visit(Less_Than_Equals_Node &n)
{
    BIN_OP_PP(n, "<=");
}
void Ast_Pretty_Printer::visit(Greater_Than_Node &n)
{
    BIN_OP_PP(n, ">");
}
void Ast_Pretty_Printer::visit(Greater_Than_Equals_Node &n)
{
    BIN_OP_PP(n, ">=");
}
void Ast_Pretty_Printer::visit(Left_Shift_Node &n)
{
    BIN_OP_PP(n, "<<");
}
void Ast_Pretty_Printer::visit(Right_Shift_Node &n)
{
    BIN_OP_PP(n, ">>");
}
void Ast_Pretty_Printer::visit(Add_Node &n)
{
    BIN_OP_PP(n, "+");
}
void Ast_Pretty_Printer::visit(Subtract_Node &n)
{
    BIN_OP_PP(n, "-");
}
void Ast_Pretty_Printer::visit(Multiply_Node &n)
{
    BIN_OP_PP(n, "*");
}
void Ast_Pretty_Printer::visit(Divide_Node &n)
{
    BIN_OP_PP(n, "/");
}
void Ast_Pretty_Printer::visit(Modulo_Node &n)
{
    BIN_OP_PP(n, "%");
}
void Ast_Pretty_Printer::visit(Call_Node &n)
{
    Ast_Pretty_Printer pp;
    str << pp.to_string(*n.lhs) << "(" << pp.to_string(*n.rhs) << ")";
}
void Ast_Pretty_Printer::visit(Index_Node &n)
{
    Ast_Pretty_Printer pp;
    str << pp.to_string(*n.lhs) << "[" << pp.to_string(*n.rhs) << "]";
}
void Ast_Pretty_Printer::visit(Field_Accessor_Node &n)
{
    Ast_Pretty_Printer pp;
    str << pp.to_string(*n.lhs) << "." << pp.to_string(*n.rhs);
}
void Ast_Pretty_Printer::visit(Negate_Node &n)
{
    Ast_Pretty_Printer pp;
    str << "(-" << pp.to_string(*n.operand) << ")";
}
void Ast_Pretty_Printer::visit(Positive_Node &n)
{
    Ast_Pretty_Printer pp;
    str << "(+" << pp.to_string(*n.operand) << ")";
}
void Ast_Pretty_Printer::visit(Not_Node &n)
{
    Ast_Pretty_Printer pp;
    str << "(!" << pp.to_string(*n.operand) << ")";
}
void Ast_Pretty_Printer::visit(Invert_Node &n)
{
    Ast_Pretty_Printer pp;
    str << "(~" << pp.to_string(*n.operand) << ")";
}
