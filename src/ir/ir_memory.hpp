#ifndef MALANG_IR_IR_MEMORY_HPP
#define MALANG_IR_IR_MEMORY_HPP

#include <vector>
#include "ir.hpp"

struct IR_Allocate_Object : IR_Node
{
    struct IR_Type *type;
    std::vector<IR_Node*> arguments; // TODO: These should be RVal

    IR_NODE_OVERRIDES;
};

struct IR_Deallocate_Object : IR_Node
{
    IR_Node *thing_to_deallocate;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_ALLOCATE_OBJECT_HPP */
