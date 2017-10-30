#ifndef MALANG_IR_IR_UNARY_OPS_HPP
#define MALANG_IR_IR_UNARY_OPS_HPP

#include "ir.hpp"
#include "ir_values.hpp"

struct IR_Unary_Operation : IR_RValue
{
    virtual ~IR_Unary_Operation();
    IR_Unary_Operation(const Source_Location &src_loc)
        : IR_RValue(src_loc)
        {}
    virtual struct Type_Info *get_type() const override;
    virtual struct Method_Info *get_method_to_call() const = 0;
    struct IR_Value *operand;
};

struct IR_U_Not : IR_Unary_Operation
{
    virtual ~IR_U_Not() = default;
    IR_U_Not(const Source_Location &src_loc)
        : IR_Unary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_U_Invert : IR_Unary_Operation
{
    virtual ~IR_U_Invert() = default;
    IR_U_Invert(const Source_Location &src_loc)
        : IR_Unary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_U_Negate : IR_Unary_Operation
{
    virtual ~IR_U_Negate() = default;
    IR_U_Negate(const Source_Location &src_loc)
        : IR_Unary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_U_Positive : IR_Unary_Operation
{
    virtual ~IR_U_Positive() = default;
    IR_U_Positive(const Source_Location &src_loc)
        : IR_Unary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};


#endif /* MALANG_IR_IR_UNARY_OPS_HPP */
