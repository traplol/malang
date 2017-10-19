#ifndef MALANG_IR_IR_LABEL_HPP
#define MALANG_IR_IR_LABEL_HPP

#include <string>
#include <vector>
#include "ir.hpp"

struct IR_Label : IR_Node
{
    IR_Label(const Source_Location &src_loc)
        : IR_Node(src_loc)
        {}
    std::string name;
    bool resolved;
    int32_t local_address;

    IR_NODE_OVERRIDES;
};

struct IR_Named_Block : IR_Label
{
    IR_Named_Block(const Source_Location &src_loc)
        : IR_Label(src_loc)
        {}
    std::vector<IR_Node*> body;
    IR_Label *end;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_LABEL_HPP */
