#ifndef MALANG_IR_IR_VRVALUE_HPP
#define MALANG_IR_IR_VRVALUE_HPP

#include "ir.hpp"

struct IR_RValue : IR_Node
{
    struct IR_Type *type;
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_VRVALUE_HPP */
