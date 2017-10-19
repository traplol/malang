#ifndef MALANG_IR_IR_UNARY_OPS_HPP
#define MALANG_IR_IR_UNARY_OPS_HPP

#include "ir.hpp"
#include "ir_values.hpp"

struct Unary_Operation : IR_RValue
{
    Unary_Operation(const Source_Location &src_loc)
        : IR_RValue(src_loc)
        {}
    virtual ~Unary_Operation(){}
    struct IR_Value *operand;
};

struct IR_U_Not : Unary_Operation
{
    IR_U_Not(const Source_Location &src_loc)
        : Unary_Operation(src_loc)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_U_Invert : Unary_Operation
{
    IR_U_Invert(const Source_Location &src_loc)
        : Unary_Operation(src_loc)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_U_Negate : Unary_Operation
{
    IR_U_Negate(const Source_Location &src_loc)
        : Unary_Operation(src_loc)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_U_Positive : Unary_Operation
{
    IR_U_Positive(const Source_Location &src_loc)
        : Unary_Operation(src_loc)
        {}
    IR_NODE_OVERRIDES;
};


#endif /* MALANG_IR_IR_UNARY_OPS_HPP */
