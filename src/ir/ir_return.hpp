#ifndef MALANG_IR_IR_RETURN_HPP
#define MALANG_IR_IR_RETURN_HPP

#include <vector>
#include "ir.hpp"

struct IR_Return : IR_Node
{
    IR_Return(const Source_Location &src_loc, const std::vector<struct IR_Value*> values, bool should_leave)
        : IR_Node(src_loc)
        , values(std::move(values))
        , should_leave(should_leave)
        {}
    std::vector<struct IR_Value*> values;
    bool should_leave;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_RETURN_HPP */
