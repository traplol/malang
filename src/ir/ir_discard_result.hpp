#ifndef MALANG_IR_IR_DISARC_RESULT_HPP
#define MALANG_IR_IR_DISARC_RESULT_HPP

#include "ir.hpp"

struct IR_Discard_Result : IR_Node
{
    virtual ~IR_Discard_Result() = default;
    IR_Discard_Result(const Source_Location &src_loc, int n)
        : IR_Node(src_loc)
        , num(n)
        {}

    IR_NODE_OVERRIDES;

    int num;
};

struct IR_Duplicate_Result : IR_Node
{
    virtual ~IR_Duplicate_Result() = default;
    IR_Duplicate_Result(const Source_Location &src_loc)
        : IR_Node(src_loc)
        {}

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_DISARC_RESULT_HPP */
