#include <stdio.h>
#include <stdint.h>
#include <sstream>

#include "ast_value.hpp"
#include "ast_variable.hpp"
#include "ast_operator.hpp"
#include "ast_list.hpp"
#include "../vm/runtime/reflection.hpp"

#define BIN_OP_DTOR(class_name)                 \
    class_name::~class_name() {                 \
        delete lhs;                             \
        lhs = nullptr;                          \
        delete rhs;                             \
        rhs = nullptr;                          \
        PRINT_DTOR;}

#define UNARY_OP_DTOR(class_name)               \
    class_name::~class_name() {                 \
        delete operand;                         \
        operand = nullptr;                      \
        PRINT_DTOR;}

#define BIN_OP_GET_TYPE(class_name, method_name)                    \
    Type_Info *class_name::get_type() {                             \
        auto lhs_type = lhs->get_type();                            \
        auto rhs_type = rhs->get_type();                            \
        if (!lhs_type || !rhs_type) {return nullptr;}               \
        auto m = lhs_type->get_method((method_name), {rhs_type});   \
        if (!m) {return nullptr;}                                   \
            return m->type()->return_type();}

#define UNARY_OP_GET_TYPE(class_name, method_name)                      \
            Type_Info *class_name::get_type() {                         \
                auto operand_type = operand->get_type();                \
                if (!operand_type) {return nullptr;}                    \
                auto m = operand_type->get_method((method_name), {});   \
                if (!m) {return nullptr;}                               \
                return m->type()->return_type();}

Type_Info *Logical_Or_Node::get_type()
{
    // @TODO: Return type(bool)
    return nullptr;
}
BIN_OP_DTOR(Logical_Or_Node)
AST_NODE_OVERRIDES_IMPL(Logical_Or_Node)

Type_Info *Logical_And_Node::get_type()
{
    // @TODO: Return type(bool)
    return nullptr;
}
BIN_OP_DTOR(Logical_And_Node)
AST_NODE_OVERRIDES_IMPL(Logical_And_Node)

BIN_OP_GET_TYPE(Inclusive_Or_Node, "b_op |")
BIN_OP_DTOR(Inclusive_Or_Node)
AST_NODE_OVERRIDES_IMPL(Inclusive_Or_Node)

BIN_OP_GET_TYPE(Exclusive_Or_Node, "b_op ^")
BIN_OP_DTOR(Exclusive_Or_Node)
AST_NODE_OVERRIDES_IMPL(Exclusive_Or_Node)

BIN_OP_GET_TYPE(And_Node, "b_op &")
BIN_OP_DTOR(And_Node)
AST_NODE_OVERRIDES_IMPL(And_Node)

BIN_OP_GET_TYPE(Equals_Node, "b_op ==")
BIN_OP_DTOR(Equals_Node)
AST_NODE_OVERRIDES_IMPL(Equals_Node)

BIN_OP_GET_TYPE(Not_Equals_Node, "b_op !=")
BIN_OP_DTOR(Not_Equals_Node)
AST_NODE_OVERRIDES_IMPL(Not_Equals_Node)

BIN_OP_GET_TYPE(Less_Than_Node, "b_op <")
BIN_OP_DTOR(Less_Than_Node)
AST_NODE_OVERRIDES_IMPL(Less_Than_Node)

BIN_OP_GET_TYPE(Less_Than_Equals_Node, "b_op <=")
BIN_OP_DTOR(Less_Than_Equals_Node)
AST_NODE_OVERRIDES_IMPL(Less_Than_Equals_Node)

BIN_OP_GET_TYPE(Greater_Than_Node, "b_op >")
BIN_OP_DTOR(Greater_Than_Node)
AST_NODE_OVERRIDES_IMPL(Greater_Than_Node)

BIN_OP_GET_TYPE(Greater_Than_Equals_Node, "b_op >=")
BIN_OP_DTOR(Greater_Than_Equals_Node)
AST_NODE_OVERRIDES_IMPL(Greater_Than_Equals_Node)

BIN_OP_GET_TYPE(Left_Shift_Node, "b_op <<")
BIN_OP_DTOR(Left_Shift_Node)
AST_NODE_OVERRIDES_IMPL(Left_Shift_Node)

BIN_OP_GET_TYPE(Right_Shift_Node, "b_op >>")
BIN_OP_DTOR(Right_Shift_Node)
AST_NODE_OVERRIDES_IMPL(Right_Shift_Node)

BIN_OP_GET_TYPE(Add_Node, "b_op +")
BIN_OP_DTOR(Add_Node)
AST_NODE_OVERRIDES_IMPL(Add_Node)

BIN_OP_GET_TYPE(Subtract_Node, "b_op -")
BIN_OP_DTOR(Subtract_Node)
AST_NODE_OVERRIDES_IMPL(Subtract_Node)

BIN_OP_GET_TYPE(Multiply_Node, "b_op *")
BIN_OP_DTOR(Multiply_Node)
AST_NODE_OVERRIDES_IMPL(Multiply_Node)

BIN_OP_GET_TYPE(Divide_Node, "b_op /")
BIN_OP_DTOR(Divide_Node)
AST_NODE_OVERRIDES_IMPL(Divide_Node)

BIN_OP_GET_TYPE(Modulo_Node, "b_op %")
BIN_OP_DTOR(Modulo_Node)
AST_NODE_OVERRIDES_IMPL(Modulo_Node)

UNARY_OP_GET_TYPE(Negate_Node, "u_op -")
UNARY_OP_DTOR(Negate_Node)
AST_NODE_OVERRIDES_IMPL(Negate_Node)

UNARY_OP_GET_TYPE(Positive_Node, "u_op +")
UNARY_OP_DTOR(Positive_Node)
AST_NODE_OVERRIDES_IMPL(Positive_Node)

UNARY_OP_GET_TYPE(Not_Node, "u_op !")
UNARY_OP_DTOR(Not_Node)
AST_NODE_OVERRIDES_IMPL(Not_Node)

UNARY_OP_GET_TYPE(Invert_Node, "u_op ~")
UNARY_OP_DTOR(Invert_Node)
AST_NODE_OVERRIDES_IMPL(Invert_Node)

AST_NODE_OVERRIDES_IMPL(Call_Node)
Call_Node::~Call_Node()
{
    delete callee;
    callee = nullptr;
    delete args;
    args = nullptr;
    PRINT_DTOR;
}
Type_Info *Call_Node::get_type()
{
    // @TODO: get_type for Call_Node
    printf("%s get_type()\n", type_name().c_str());
    return nullptr;
}

AST_NODE_OVERRIDES_IMPL(Index_Node)
Index_Node::~Index_Node()
{
    delete thing;
    thing = nullptr;
    delete subscript;
    subscript = nullptr;
    PRINT_DTOR;
}
Type_Info *Index_Node::get_type()
{
    // @TODO: get_type for Index_Node
    return nullptr;
}

AST_NODE_OVERRIDES_IMPL(Field_Accessor_Node)
Field_Accessor_Node::~Field_Accessor_Node()
{
    delete thing;
    thing = nullptr;
    delete member;
    member = nullptr;
    PRINT_DTOR;
}
Type_Info *Field_Accessor_Node::get_type()
{
    auto thing_ty = thing->get_type();
    if (!thing_ty)
    {
        return nullptr;
    }
    auto field_info = thing_ty->get_field(member->name);
    if (!field_info)
    {
        return nullptr;
    }
    return field_info->type();
}
