#ifndef MALANG_IR_IR_RETURN_HPP
#define MALANG_IR_IR_RETURN_HPP

#include <vector>
#include "ir.hpp"

struct IR_Return : IR_Node
{
    IR_Return(const Source_Location &src_loc)
        : IR_Node(src_loc)
        {}
    std::vector<struct IR_Value*> values;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_RETURN_HPP */
