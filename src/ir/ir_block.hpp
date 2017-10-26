#ifndef MALANG_IR_IR_BLOCK_HPP
#define MALANG_IR_IR_BLOCK_HPP

#include <vector>
#include "ir.hpp"
#include "ir_values.hpp"

struct IR_Block : IR_RValue
{
    virtual ~IR_Block();
    IR_Block(const Source_Location &src_loc, const std::vector<IR_Node*> &nodes, Type_Info *type)
        : IR_RValue(src_loc)
        , nodes(std::move(nodes))
        , type(type)
        {}
    IR_NODE_OVERRIDES;
    std::vector<IR_Node*> nodes;
    Type_Info *type;
    virtual Type_Info *get_type() const override;
};

#endif /* MALANG_IR_IR_BLOCK_HPP */
