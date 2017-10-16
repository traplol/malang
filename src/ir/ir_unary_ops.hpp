#ifndef MALANG_IR_IR_UNARY_OPS_HPP
#define MALANG_IR_IR_UNARY_OPS_HPP

#include "ir.hpp"

struct Unary_Operation : IR_Node
{
    virtual ~Unary_Operation(){}
    struct IR_RValue *operand;
};

struct IR_U_Not : Unary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_U_Invert : Unary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_U_Negate : Unary_Operation
{
    IR_NODE_OVERRIDES;
};

struct IR_U_Positive : Unary_Operation
{
    IR_NODE_OVERRIDES;
};


#endif /* MALANG_IR_IR_UNARY_OPS_HPP */
