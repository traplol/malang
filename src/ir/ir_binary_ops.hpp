#ifndef MALANG_IR_IR_BINARY_OPS_HPP
#define MALANG_IR_IR_BINARY_OPS_HPP

#include "ir.hpp"

struct IR_Binary_Operation : IR_Node
{
    virtual ~IR_Binary_Operation() {}
    struct IR_RValue *lhs;
    struct IR_RValue *rhs;
};

struct IR_B_Add : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Subtract : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Multiply : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Divide : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Modulo : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_And : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Or : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Xor : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Left_Shift : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Right_Shift : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Less_Than : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Less_Than_Equals : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Greater_Than : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Greater_Than_Equals : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_B_Equals : IR_Binary_Operation
{
    IR_NODE_OVERRIDES;
};


#endif /* MALANG_IR_IR_BINARY_OPS_HPP */
