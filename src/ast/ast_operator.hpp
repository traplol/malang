#ifndef MALANG_AST_OPERATOR_AST_HPP
#define MALANG_AST_OPERATOR_AST_HPP

#include "ast.hpp"


#define DEF_BINARY_AST_NODE(class_name) \
struct class_name : public Binary_Holder { \
    ~class_name(); \
    class_name(Ast_Node *lhs, Ast_Node *rhs) {this->lhs = lhs; this->rhs = rhs;} \
    METADATA_OVERRIDES; \
}

#define DEF_PREFIX_AST_NODE(class_name) \
struct class_name : public Ast_Node { \
    Ast_Node *rhs; \
    ~class_name(); \
    class_name(Ast_Node *rhs) : rhs(rhs) {} \
    METADATA_OVERRIDES; \
}

struct Binary_Holder : public Ast_Node
{
    Ast_Node *lhs;
    Ast_Node *rhs;
};

DEF_BINARY_AST_NODE(Logical_Or_Node);
DEF_BINARY_AST_NODE(Logical_And_Node);
DEF_BINARY_AST_NODE(Inclusive_Or_Node);
DEF_BINARY_AST_NODE(Exclusive_Or_Node);
DEF_BINARY_AST_NODE(And_Node);
DEF_BINARY_AST_NODE(Equals_Node);
DEF_BINARY_AST_NODE(Not_Equals_Node);
DEF_BINARY_AST_NODE(Less_Than_Node);
DEF_BINARY_AST_NODE(Less_Than_Equals_Node);
DEF_BINARY_AST_NODE(Greater_Than_Node);
DEF_BINARY_AST_NODE(Greater_Than_Equals_Node);
DEF_BINARY_AST_NODE(Left_Shift_Node);
DEF_BINARY_AST_NODE(Right_Shift_Node);
DEF_BINARY_AST_NODE(Add_Node);
DEF_BINARY_AST_NODE(Subtract_Node);
DEF_BINARY_AST_NODE(Multiply_Node);
DEF_BINARY_AST_NODE(Divide_Node);
DEF_BINARY_AST_NODE(Modulo_Node);

DEF_BINARY_AST_NODE(Call_Node);
DEF_BINARY_AST_NODE(Index_Node);
DEF_BINARY_AST_NODE(Field_Accessor_Node);

DEF_PREFIX_AST_NODE(Negate_Node);
DEF_PREFIX_AST_NODE(Positive_Node);
DEF_PREFIX_AST_NODE(Not_Node);
DEF_PREFIX_AST_NODE(Invert_Node);

#endif /* MALANG_AST_OPERATOR_AST_HPP */
