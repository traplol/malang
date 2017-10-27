#ifndef MALANG_IR_IR_MEMORY_HPP
#define MALANG_IR_IR_MEMORY_HPP

#include <vector>
#include "ir.hpp"

struct IR_Allocate_Object : IR_Node // @FixMe: should this be IR_RValue?
{
    virtual ~IR_Allocate_Object();
    IR_Allocate_Object(const Source_Location &src_loc)
        : IR_Node(src_loc)
        {}
    struct Type_Info *type;
    std::vector<struct IR_Value*> arguments;

    IR_NODE_OVERRIDES;
};

struct IR_Deallocate_Object : IR_Node
{
    virtual ~IR_Deallocate_Object();
    IR_Deallocate_Object(const Source_Location &src_loc)
        : IR_Node(src_loc)
        {}
    IR_Value *thing_to_deallocate;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_ALLOCATE_OBJECT_HPP */
