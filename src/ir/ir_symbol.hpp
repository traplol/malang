#ifndef MALANG_IR_IR_SYMBOL_HPP
#define MALANG_IR_IR_SYMBOL_HPP

#include <string>
#include "ir.hpp"
#include "ir_values.hpp"

struct IR_Symbol : IR_LValue
{
    IR_Symbol(const Source_Location &src_loc)
        : IR_LValue(src_loc)
        {}
    std::string symbol;
    size_t index;
    struct Type_Info *type;
    virtual struct Type_Info *get_type() const override { return type; }

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_SYMBOL_HPP */
