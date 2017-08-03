#include <stdio.h>
#include <stdint.h>
#include <sstream>

#include "ast_value.hpp"
#include "ast_variable.hpp"
#include "ast_operator.hpp"
#include "ast_list.hpp"

#define if_BINARY_INT_INT_OP(a, b, c, op)                       \
    if (a->node_type_id() == Integer_Node::type_id() &&         \
        b->node_type_id() == Integer_Node::type_id()) {         \
        auto ai = reinterpret_cast<Integer_Node*>(a)->value;    \
        auto bi = reinterpret_cast<Integer_Node*>(b)->value;    \
        decltype(ai) ci = ai op bi;                             \
        printf("%ld %s %ld = %ld ~> ", ai, #op, bi, ci);        \
        c = new Integer_Node(ci);}

#define if_PREFIX_INT_OP(a, b, op)                              \
    if (a->node_type_id() == Integer_Node::type_id()) {         \
        auto ai = reinterpret_cast<Integer_Node*>(a)->value;    \
        decltype(ai) bi = op ai;                                \
        printf("%s%ld = %ld ~> ", #op, ai, bi);                 \
        b = new Integer_Node(bi);}

#define BIN_OP_DTOR(class_name) \
    class_name::~class_name() { \
        delete lhs;             \
        lhs = nullptr;          \
        delete rhs;             \
        rhs = nullptr;          \
        PRINT_DTOR;}

#define BIN_OP_TO_STRING(class_name, op)                                \
    std::string class_name::to_string() const {                         \
        std::stringstream ss;                                           \
        ss << "(" << lhs->to_string() << " " op " " << rhs->to_string() << ")"; \
        return ss.str();}

#define PREFIX_OP_DTOR(class_name) \
    class_name::~class_name() { \
        delete rhs;             \
        rhs = nullptr;          \
        PRINT_DTOR;}

#define PREFIX_OP_TO_STRING(class_name, op)                                \
    std::string class_name::to_string() const {                         \
        std::stringstream ss;                                           \
        ss << "(" op << rhs->to_string() << ")"; \
        return ss.str();}

BIN_OP_DTOR(Logical_Or_Node)
BIN_OP_TO_STRING(Logical_Or_Node, "||")
AST_NODE_OVERRIDES_IMPL(Logical_Or_Node)
{
    lhs->execute(ctx);
    rhs->execute(ctx);
    printf("Executed %s\n", node_name().c_str());
    return nullptr;
}
BIN_OP_DTOR(Logical_And_Node)
BIN_OP_TO_STRING(Logical_And_Node, "&&")
AST_NODE_OVERRIDES_IMPL(Logical_And_Node)
{
    lhs->execute(ctx);
    rhs->execute(ctx);
    printf("Executed %s\n", node_name().c_str());
    return nullptr;
}
BIN_OP_DTOR(Inclusive_Or_Node)
BIN_OP_TO_STRING(Inclusive_Or_Node, "|")
AST_NODE_OVERRIDES_IMPL(Inclusive_Or_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, |)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Exclusive_Or_Node)
BIN_OP_TO_STRING(Exclusive_Or_Node, "^")
AST_NODE_OVERRIDES_IMPL(Exclusive_Or_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, ^)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(And_Node)
BIN_OP_TO_STRING(And_Node, "&")
AST_NODE_OVERRIDES_IMPL(And_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, &)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Equals_Node)
BIN_OP_TO_STRING(Equals_Node, "==")
AST_NODE_OVERRIDES_IMPL(Equals_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, ==)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Not_Equals_Node)
BIN_OP_TO_STRING(Not_Equals_Node, "!=")
AST_NODE_OVERRIDES_IMPL(Not_Equals_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, !=)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Less_Than_Node)
BIN_OP_TO_STRING(Less_Than_Node, "<")
AST_NODE_OVERRIDES_IMPL(Less_Than_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, <)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Less_Than_Equals_Node)
BIN_OP_TO_STRING(Less_Than_Equals_Node, "<=")
AST_NODE_OVERRIDES_IMPL(Less_Than_Equals_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, <=)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Greater_Than_Node)
BIN_OP_TO_STRING(Greater_Than_Node, ">")
AST_NODE_OVERRIDES_IMPL(Greater_Than_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, >)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Greater_Than_Equals_Node)
BIN_OP_TO_STRING(Greater_Than_Equals_Node, ">=")
AST_NODE_OVERRIDES_IMPL(Greater_Than_Equals_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, >=)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Left_Shift_Node)
BIN_OP_TO_STRING(Left_Shift_Node, "<<")
AST_NODE_OVERRIDES_IMPL(Left_Shift_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, <<)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Right_Shift_Node)
BIN_OP_TO_STRING(Right_Shift_Node, ">>")
AST_NODE_OVERRIDES_IMPL(Right_Shift_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, >>)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Add_Node)
BIN_OP_TO_STRING(Add_Node, "+")
AST_NODE_OVERRIDES_IMPL(Add_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, +)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Subtract_Node)
BIN_OP_TO_STRING(Subtract_Node, "-")
AST_NODE_OVERRIDES_IMPL(Subtract_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, -)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Multiply_Node)
BIN_OP_TO_STRING(Multiply_Node, "*")
AST_NODE_OVERRIDES_IMPL(Multiply_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, *)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Divide_Node)
BIN_OP_TO_STRING(Divide_Node, "/")
AST_NODE_OVERRIDES_IMPL(Divide_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, /)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
BIN_OP_DTOR(Modulo_Node)
BIN_OP_TO_STRING(Modulo_Node, "%")
AST_NODE_OVERRIDES_IMPL(Modulo_Node)
{
    auto a = lhs->execute(ctx);
    auto b = rhs->execute(ctx);
    Ast_Node *c = nullptr;
    if_BINARY_INT_INT_OP(a, b, c, %)
    printf("Executed %s\n", node_name().c_str());
    return c;
}
PREFIX_OP_DTOR(Negate_Node)
PREFIX_OP_TO_STRING(Negate_Node, "-")
AST_NODE_OVERRIDES_IMPL(Negate_Node)
{
    auto a = rhs->execute(ctx);
    Ast_Node *b = nullptr;
    if_PREFIX_INT_OP(a, b, -)
    printf("Executed %s\n", node_name().c_str());
    return b;
}
PREFIX_OP_DTOR(Positive_Node)
PREFIX_OP_TO_STRING(Positive_Node, "+")
AST_NODE_OVERRIDES_IMPL(Positive_Node)
{
    auto a = rhs->execute(ctx);
    Ast_Node *b = nullptr;
    if_PREFIX_INT_OP(a, b, +)
    printf("Executed %s\n", node_name().c_str());
    return b;
}
PREFIX_OP_DTOR(Not_Node)
PREFIX_OP_TO_STRING(Not_Node, "!")
AST_NODE_OVERRIDES_IMPL(Not_Node)
{
    auto a = rhs->execute(ctx);
    Ast_Node *b = nullptr;
    if_PREFIX_INT_OP(a, b, !)
    printf("Executed %s\n", node_name().c_str());
    return b;
}
PREFIX_OP_DTOR(Invert_Node)
PREFIX_OP_TO_STRING(Invert_Node, "~")
AST_NODE_OVERRIDES_IMPL(Invert_Node)
{
    auto a = rhs->execute(ctx);
    Ast_Node *b = nullptr;
    if_PREFIX_INT_OP(a, b, ~)
    printf("Executed %s\n", node_name().c_str());
    return b;
}
BIN_OP_DTOR(Call_Node)
std::string Call_Node::to_string() const
{
    std::stringstream ss;
    ss << lhs->to_string() << "(" << rhs->to_string() << ")";
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Call_Node)
{
    auto callee = lhs->execute(ctx);
    auto args = reinterpret_cast<List_Node*>(rhs->execute(ctx));
    if (callee->node_type_id() == Variable_Node::type_id())
    {
        auto var_name = reinterpret_cast<Variable_Node*>(callee)->name;
        if (var_name == "print")
        {
            for (auto &&n : args->contents)
            {
                printf("%s\n", n->execute(ctx)->to_string().c_str());
            }
        }
    }
    printf("Executed %s (%s)\n", node_name().c_str(), callee->to_string().c_str());
    return nullptr;
}
BIN_OP_DTOR(Index_Node)
std::string Index_Node::to_string() const
{
    std::stringstream ss;
    ss << lhs->to_string() << "[" << rhs->to_string() << "]";
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Index_Node)
{
    lhs->execute(ctx);
    rhs->execute(ctx);
    printf("Executed %s\n", node_name().c_str());
    return nullptr;
}
BIN_OP_DTOR(Field_Accessor_Node)
std::string Field_Accessor_Node::to_string() const
{
    std::stringstream ss;
    ss << lhs->to_string() << "." << rhs->to_string();
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Field_Accessor_Node)
{
    lhs->execute(ctx);
    rhs->execute(ctx);
    printf("Executed %s\n", node_name().c_str());
    return nullptr;
}
