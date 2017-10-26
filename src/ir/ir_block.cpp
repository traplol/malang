#include "ir_block.hpp"

IR_Block::~IR_Block()
{
    for (auto &&n : nodes)
    {
        delete n;
    }
    nodes.clear();
}

IR_NODE_OVERRIDES_IMPL(IR_Block);

Type_Info *IR_Block::get_type() const
{
    return type;
}
