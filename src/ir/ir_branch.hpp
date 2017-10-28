#ifndef MALANG_IR_IR_BRANCH_HPP
#define MALANG_IR_IR_BRANCH_HPP

#include "ir.hpp"
#include "ir_label.hpp"

struct IR_Branch : IR_Node
{
    virtual ~IR_Branch();
    IR_Branch(const Source_Location &src_loc, IR_Label *dest)
        : IR_Node(src_loc)
        , destination(dest)
        {}
    IR_Label *destination;
    IR_NODE_OVERRIDES;
};

struct IR_Pop_Branch_If_True : IR_Branch
{
    virtual ~IR_Pop_Branch_If_True() = default;
    IR_Pop_Branch_If_True(const Source_Location &src_loc, IR_Label *dest)
        : IR_Branch(src_loc, dest)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_Pop_Branch_If_False : IR_Branch
{
    virtual ~IR_Pop_Branch_If_False() = default;
    IR_Pop_Branch_If_False(const Source_Location &src_loc, IR_Label *dest)
        : IR_Branch(src_loc, dest)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_Branch_If_True_Or_Pop : IR_Branch
{
    virtual ~IR_Branch_If_True_Or_Pop() = default;
    IR_Branch_If_True_Or_Pop(const Source_Location &src_loc, IR_Label *dest)
        : IR_Branch(src_loc, dest)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_Branch_If_False_Or_Pop : IR_Branch
{
    virtual ~IR_Branch_If_False_Or_Pop() = default;
    IR_Branch_If_False_Or_Pop(const Source_Location &src_loc, IR_Label *dest)
        : IR_Branch(src_loc, dest)
        {}
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_BRANCH_HPP */
