#ifndef MALANG_IR_IR_BINARY_OPS_HPP
#define MALANG_IR_IR_BINARY_OPS_HPP

#include "ir.hpp"
#include "ir_values.hpp"

struct IR_Binary_Operation : IR_RValue
{
    IR_Binary_Operation(const Source_Location &src_loc)
        : IR_RValue(src_loc)
        {}
    virtual ~IR_Binary_Operation() {}
    virtual struct Type_Info *get_type() const override;
    virtual struct Method_Info *get_method_to_call() const = 0;
    struct IR_Value *lhs;
    struct IR_Value *rhs;
};

struct IR_B_Add : IR_Binary_Operation
{
    IR_B_Add(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Subtract : IR_Binary_Operation
{
    IR_B_Subtract(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Multiply : IR_Binary_Operation
{
    IR_B_Multiply(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Divide : IR_Binary_Operation
{
    IR_B_Divide(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Modulo : IR_Binary_Operation
{
    IR_B_Modulo(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_And : IR_Binary_Operation
{
    IR_B_And(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Or : IR_Binary_Operation
{
    IR_B_Or(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Xor : IR_Binary_Operation
{
    IR_B_Xor(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Left_Shift : IR_Binary_Operation
{
    IR_B_Left_Shift(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Right_Shift : IR_Binary_Operation
{
    IR_B_Right_Shift(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Less_Than : IR_Binary_Operation
{
    IR_B_Less_Than(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Less_Than_Equals : IR_Binary_Operation
{
    IR_B_Less_Than_Equals(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Greater_Than : IR_Binary_Operation
{
    IR_B_Greater_Than(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Greater_Than_Equals : IR_Binary_Operation
{
    IR_B_Greater_Than_Equals(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Equals : IR_Binary_Operation
{
    IR_B_Equals(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

struct IR_B_Not_Equals : IR_Binary_Operation
{
    IR_B_Not_Equals(const Source_Location &src_loc)
        : IR_Binary_Operation(src_loc)
        {}
    virtual struct Method_Info *get_method_to_call() const override;
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_BINARY_OPS_HPP */
