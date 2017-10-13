#ifndef MALANG_IR_IR_BRANCH_HPP
#define MALANG_IR_IR_BRANCH_HPP

#include "ir.hpp"

struct IR_Branch : IR_Node
{
    struct IR_Label *destination;
    IR_NODE_OVERRIDES;
};

struct IR_Branch_If_True : IR_Branch
{
    IR_NODE_OVERRIDES;
};

struct IR_Branch_If_False : IR_Branch
{
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_BRANCH_HPP */
