#ifndef MALANG_AST_OPERATOR_AST_HPP
#define MALANG_AST_OPERATOR_AST_HPP

#include "ast_value.hpp"


#define DEF_BINARY_AST_NODE(class_name)                                 \
    struct class_name : public Binary_Holder {                          \
        ~class_name();                                                  \
        class_name(const Source_Location &src_loc, Ast_Value *lhs, Ast_Value *rhs) \
            : Binary_Holder(src_loc, lhs, rhs) {}                       \
        virtual Type_Info *get_type() final;                            \
        AST_NODE_OVERRIDES;                                             \
    }

#define DEF_PREFIX_AST_NODE(class_name)                                 \
    struct class_name : public Ast_RValue {                             \
        Ast_Value *operand;                                             \
        ~class_name();                                                  \
        class_name(const Source_Location &src_loc, Ast_Value *operand)  \
            : Ast_RValue(src_loc)                                       \
            , operand(operand) {}                                       \
        virtual Type_Info *get_type() final;                            \
        AST_NODE_OVERRIDES;                                             \
    }

struct Binary_Holder : public Ast_RValue
{
    Ast_Value *lhs;
    Ast_Value *rhs;
    Binary_Holder(const Source_Location &src_loc, Ast_Value *lhs, Ast_Value *rhs)
        : Ast_RValue(src_loc)
        , lhs(lhs)
        , rhs(rhs)
    {}
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

struct Call_Node : public Ast_RValue
{
    Ast_Value *callee; 
    List_Node *args;
    ~Call_Node();
    Call_Node(const Source_Location &src_loc, Ast_Value *callee, List_Node *args)
        : Ast_RValue(src_loc)
        , callee(callee)
        , args(args)
        {}
    virtual Type_Info *get_type() final;
    AST_NODE_OVERRIDES;
};

struct Index_Node : public Ast_LValue
{
    Ast_Value *thing; 
    Ast_Value *subscript;
    ~Index_Node();
    Index_Node(const Source_Location &src_loc, Ast_Value *thing, Ast_Value *subscript)
        : Ast_LValue(src_loc)
        , thing(thing)
        , subscript(subscript)
        {}
    //virtual Type_Info *get_type() final;
    AST_NODE_OVERRIDES;
};

struct Member_Accessor_Node : public Ast_LValue
{
    Ast_Value *thing; 
    Variable_Node *member;
    ~Member_Accessor_Node();
    Member_Accessor_Node(const Source_Location &src_loc, Ast_Value *thing, Variable_Node *member)
        : Ast_LValue(src_loc)
        , thing(thing)
        , member(member)
        {}
    virtual Type_Info *get_type() final;
    AST_NODE_OVERRIDES;
};

DEF_PREFIX_AST_NODE(Negate_Node);
DEF_PREFIX_AST_NODE(Positive_Node);
DEF_PREFIX_AST_NODE(Not_Node);
DEF_PREFIX_AST_NODE(Invert_Node);

#endif /* MALANG_AST_OPERATOR_AST_HPP */
