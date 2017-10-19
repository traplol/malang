#ifndef MALANG_IR_IR_CALL_HPP
#define MALANG_IR_IR_CALL_HPP

#include <vector>
#include "ir.hpp"
#include "ir_values.hpp"

struct IR_Call : IR_RValue
{
    IR_Call(const Source_Location &src_loc)
        : IR_RValue(src_loc)
        {}

    struct IR_Value *callee;
    std::vector<struct IR_Value*> arguments;

    IR_NODE_OVERRIDES;
};

struct IR_Call_Method : IR_Call
{
    IR_Call_Method(const Source_Location &src_loc)
        : IR_Call(src_loc)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_Call_Virtual_Method : IR_Call
{
    IR_Call_Virtual_Method(const Source_Location &src_loc)
        : IR_Call(src_loc)
        {}
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_CALL_HPP */
