#ifndef MALANG_IR_IR_CALL_HPP
#define MALANG_IR_IR_CALL_HPP

#include <vector>
#include "ir.hpp"

struct Call_Arg
{
    struct IR_Type *type;
    IR_Node *value;
};

struct IR_Call : IR_Node
{
    struct IR_Callable *callee;
    std::vector<Call_Arg> arguments;

    IR_NODE_OVERRIDES;
};

struct IR_Call_Method : IR_Call
{
    IR_NODE_OVERRIDES;
};

struct IR_Call_Virtual_Method : IR_Call
{
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_CALL_HPP */
