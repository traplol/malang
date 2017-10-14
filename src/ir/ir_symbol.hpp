#ifndef MALANG_IR_IR_SYMBOL_HPP
#define MALANG_IR_IR_SYMBOL_HPP

#include <string>
#include "ir.hpp"

struct IR_Symbol : IR_Node
{
    struct IR_Type *type;
    std::string symbol;
    size_t index;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_SYMBOL_HPP */
