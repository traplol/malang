#ifndef MALANG_IR_IR_TYPE_HPP
#define MALANG_IR_IR_TYPE_HPP

#include <string>
#include <vector>
#include "ir.hpp"

struct Method
{
    struct IR_Symbol *symbol;
    struct IR_Type *return_type;
    std::vector<IR_Symbol*> parameters;
};

struct IR_Type : IR_Node
{
    IR_Type *parent;
    std::string name;
    size_t index;
    std::vector<struct IR_Symbol*> fields;
    std::vector<Method> methods;
    std::vector<Method> virtual_methods;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_TYPE_HPP */
