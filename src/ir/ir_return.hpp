#ifndef MALANG_IR_IR_RETURN_HPP
#define MALANG_IR_IR_RETURN_HPP

#include <vector>
#include "ir.hpp"

struct IR_Return : IR_Node
{
    std::vector<IR_Node*> values;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_RETURN_HPP */
