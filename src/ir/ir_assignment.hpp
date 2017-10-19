#ifndef MALANG_IR_IR_ASSIGNMENT
#define MALANG_IR_IR_ASSIGNMENT

#include "ir.hpp"

struct IR_Assignment : IR_Node
{
    IR_Assignment(const Source_Location &src_loc)
        : IR_Node(src_loc)
        {}
    struct IR_LValue *lhs;
    struct IR_RValue *rhs;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_ASSIGNMENT */
