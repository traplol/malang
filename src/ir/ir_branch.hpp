#ifndef MALANG_IR_IR_BRANCH_HPP
#define MALANG_IR_IR_BRANCH_HPP

#include "ir.hpp"

struct IR_Branch : IR_Node
{
    IR_Branch(const Source_Location &src_loc, struct IR_Label *dest)
        : IR_Node(src_loc)
        , destination(dest)
        {}
    struct IR_Label *destination;
    IR_NODE_OVERRIDES;
};

struct IR_Branch_If_True : IR_Branch
{
    IR_Branch_If_True(const Source_Location &src_loc, struct IR_Label *dest)
        : IR_Branch(src_loc, dest)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_Branch_If_False : IR_Branch
{
    IR_Branch_If_False(const Source_Location &src_loc, struct IR_Label *dest)
        : IR_Branch(src_loc, dest)
        {}
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_BRANCH_HPP */
