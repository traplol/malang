#ifndef MALANG_IR_IR_ASSIGNMENT
#define MALANG_IR_IR_ASSIGNMENT

#include "ir.hpp"

struct IR_Assignment : IR_Node
{
    // TODO: LHS should be LVal
    IR_Node *lhs;
    // TODO: RHS should be RVal
    IR_Node *rhs;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_ASSIGNMENT */
