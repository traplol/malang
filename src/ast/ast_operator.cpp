#include <stdio.h>
#include <stdint.h>
#include <sstream>

#include "ast_value.hpp"
#include "ast_variable.hpp"
#include "ast_operator.hpp"
#include "ast_list.hpp"

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

BIN_OP_DTOR(Logical_And_Node)
BIN_OP_TO_STRING(Logical_And_Node, "&&")
AST_NODE_OVERRIDES_IMPL(Logical_And_Node)

BIN_OP_DTOR(Inclusive_Or_Node)
BIN_OP_TO_STRING(Inclusive_Or_Node, "|")
AST_NODE_OVERRIDES_IMPL(Inclusive_Or_Node)

BIN_OP_DTOR(Exclusive_Or_Node)
BIN_OP_TO_STRING(Exclusive_Or_Node, "^")
AST_NODE_OVERRIDES_IMPL(Exclusive_Or_Node)

BIN_OP_DTOR(And_Node)
BIN_OP_TO_STRING(And_Node, "&")
AST_NODE_OVERRIDES_IMPL(And_Node)

BIN_OP_DTOR(Equals_Node)
BIN_OP_TO_STRING(Equals_Node, "==")
AST_NODE_OVERRIDES_IMPL(Equals_Node)

BIN_OP_DTOR(Not_Equals_Node)
BIN_OP_TO_STRING(Not_Equals_Node, "!=")
AST_NODE_OVERRIDES_IMPL(Not_Equals_Node)

BIN_OP_DTOR(Less_Than_Node)
BIN_OP_TO_STRING(Less_Than_Node, "<")
AST_NODE_OVERRIDES_IMPL(Less_Than_Node)

BIN_OP_DTOR(Less_Than_Equals_Node)
BIN_OP_TO_STRING(Less_Than_Equals_Node, "<=")
AST_NODE_OVERRIDES_IMPL(Less_Than_Equals_Node)

BIN_OP_DTOR(Greater_Than_Node)
BIN_OP_TO_STRING(Greater_Than_Node, ">")
AST_NODE_OVERRIDES_IMPL(Greater_Than_Node)

BIN_OP_DTOR(Greater_Than_Equals_Node)
BIN_OP_TO_STRING(Greater_Than_Equals_Node, ">=")
AST_NODE_OVERRIDES_IMPL(Greater_Than_Equals_Node)

BIN_OP_DTOR(Left_Shift_Node)
BIN_OP_TO_STRING(Left_Shift_Node, "<<")
AST_NODE_OVERRIDES_IMPL(Left_Shift_Node)

BIN_OP_DTOR(Right_Shift_Node)
BIN_OP_TO_STRING(Right_Shift_Node, ">>")
AST_NODE_OVERRIDES_IMPL(Right_Shift_Node)

BIN_OP_DTOR(Add_Node)
BIN_OP_TO_STRING(Add_Node, "+")
AST_NODE_OVERRIDES_IMPL(Add_Node)

BIN_OP_DTOR(Subtract_Node)
BIN_OP_TO_STRING(Subtract_Node, "-")
AST_NODE_OVERRIDES_IMPL(Subtract_Node)

BIN_OP_DTOR(Multiply_Node)
BIN_OP_TO_STRING(Multiply_Node, "*")
AST_NODE_OVERRIDES_IMPL(Multiply_Node)

BIN_OP_DTOR(Divide_Node)
BIN_OP_TO_STRING(Divide_Node, "/")
AST_NODE_OVERRIDES_IMPL(Divide_Node)

BIN_OP_DTOR(Modulo_Node)
BIN_OP_TO_STRING(Modulo_Node, "%")
AST_NODE_OVERRIDES_IMPL(Modulo_Node)

PREFIX_OP_DTOR(Negate_Node)
PREFIX_OP_TO_STRING(Negate_Node, "-")
AST_NODE_OVERRIDES_IMPL(Negate_Node)

PREFIX_OP_DTOR(Positive_Node)
PREFIX_OP_TO_STRING(Positive_Node, "+")
AST_NODE_OVERRIDES_IMPL(Positive_Node)

PREFIX_OP_DTOR(Not_Node)
PREFIX_OP_TO_STRING(Not_Node, "!")
AST_NODE_OVERRIDES_IMPL(Not_Node)

PREFIX_OP_DTOR(Invert_Node)
PREFIX_OP_TO_STRING(Invert_Node, "~")
AST_NODE_OVERRIDES_IMPL(Invert_Node)

BIN_OP_DTOR(Call_Node)
std::string Call_Node::to_string() const
{
    std::stringstream ss;
    ss << lhs->to_string() << "(" << rhs->to_string() << ")";
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Call_Node)

BIN_OP_DTOR(Index_Node)
std::string Index_Node::to_string() const
{
    std::stringstream ss;
    ss << lhs->to_string() << "[" << rhs->to_string() << "]";
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Index_Node)

BIN_OP_DTOR(Field_Accessor_Node)
std::string Field_Accessor_Node::to_string() const
{
    std::stringstream ss;
    ss << lhs->to_string() << "." << rhs->to_string();
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Field_Accessor_Node)

