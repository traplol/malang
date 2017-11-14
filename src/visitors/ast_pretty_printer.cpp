#include <cassert>
#include "ast_pretty_printer.hpp"
#include "../ast/nodes.hpp"
#include "../lexer.hpp"
#include "../module_map.hpp"


std::string Ast_Pretty_Printer::to_string(Ast_Node &n)
{
    n.accept(*this);
    return str.str();
}
std::vector<std::string> Ast_Pretty_Printer::to_strings(Ast &ast)
{
    std::vector<std::string> res;
    for (auto import : ast.imports)
    {
        to_string(*import);
        res.push_back(str.str());
        reset();
    }
    for (auto type : ast.type_defs)
    {
        to_string(*type);
        res.push_back(str.str());
        reset();
    }
    for (auto extend : ast.extensions)
    {
        to_string(*extend);
        res.push_back(str.str());
        reset();
    }
    for (auto bound_fn : ast.bound_funcs)
    {
        to_string(*bound_fn);
        res.push_back(str.str());
        reset();
    }
    for (auto stmt : ast.stmts)
    {
        to_string(*stmt);
        res.push_back(str.str());
        reset();
    }
    return res;
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

void Ast_Pretty_Printer::visit(Import_Node &n)
{
    assert(n.mod_info);
    str << "import " << n.mod_info->fully_qualified_name();
}
void Ast_Pretty_Printer::visit(Variable_Node &n)
{
    str << n.name();
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
    if (n.is_bound())
    {
        str << "fn " << n.bound_name << "(";
    }
    else
    {
        str << "fn (";
    }
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
    str << '"';
    for (auto c : n.value)
    {
        str << Lexer::escape(c);
    }
    str << '"';
}
void Ast_Pretty_Printer::visit(Boolean_Node &n)
{
    str << (n.value ? "true" : "false");
}
void Ast_Pretty_Printer::visit(Character_Node &n)
{
    str << "?" << Lexer::escape(n.value);
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
void Ast_Pretty_Printer::visit(Member_Accessor_Node &n)
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
void Ast_Pretty_Printer::visit(Constructor_Node &n)
{
    str << "new (";
    for (size_t i = 0; i < n.params.size(); ++i)
    {
        assert(n.params[i]);
        to_string(*n.params[i]);
        if (i + 1 < n.params.size())
        {
            str << ", ";
        }
    }
    str << ")";
    do_body(n.body);
}
void Ast_Pretty_Printer::visit(Type_Def_Node &n)
{
    assert(n.type);
    str << "type " << n.type->name() << " = {\n";
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
void Ast_Pretty_Printer::visit(Extend_Node &n)
{
    assert(n.for_type);
    str << "extend "; to_string(*n.for_type); str << " {\n";
    indent();
    for (auto &&fn : n.body)
    {
        do_indention();
        to_string(*fn);
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
void Ast_Pretty_Printer::visit(struct Break_Node &n)
{
    str << "break";
    if (!n.values->contents.empty())
    {
        str << " ";
        to_string(*n.values);
    }
}
void Ast_Pretty_Printer::visit(struct Continue_Node &)
{
    str << "continue";
}
void Ast_Pretty_Printer::visit(struct While_Node &n)
{
    assert(n.condition);
    str << "while "; to_string(*n.condition); do_body(n.body);
}
void Ast_Pretty_Printer::visit(struct For_Node &n)
{
    assert(n.iterable);
    str << "for "; to_string(*n.iterable); do_body(n.body);
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

void Ast_Pretty_Printer::visit(struct Array_Literal_Node &n)
{
    assert(n.values);
    str << "["; to_string(*n.values); str << "]";
}

void Ast_Pretty_Printer::visit(struct New_Array_Node &n)
{
    assert(n.size);
    assert(n.of_type);
    str << "["; to_string(*n.size); str << "]" << n.of_type->type->name();
}
