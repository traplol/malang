#ifndef MALANG_IR_IR_ASSIGNMENT
#define MALANG_IR_IR_ASSIGNMENT

#include "ir.hpp"

struct IR_Assignment : IR_Node
{
    struct IR_LValue *lhs;
    struct IR_RValue *rhs;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_ASSIGNMENT */
