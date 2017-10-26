#ifndef MALANG_IR_IR_NOOP_HPP
#define MALANG_IR_IR_NOOP_HPP

#include "ir.hpp"

struct IR_Noop : IR_Node
{
    virtual ~IR_Noop() {}
    IR_Noop(const Source_Location &src_loc)
        : IR_Node(src_loc)
        {}
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_NOOP_HPP */
