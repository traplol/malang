#ifndef MALANG_IR_IR_PRIMITIVES_HPP
#define MALANG_IR_IR_PRIMITIVES_HPP

#include <vector>
#include <string>
#include <stdint.h>
#include "ir.hpp"

struct IR_Boolean : IR_Node
{
    bool value;
    
    IR_NODE_OVERRIDES;
};

struct IR_Char : IR_Node
{
    char value;
    
    IR_NODE_OVERRIDES;
};

struct IR_Fixnum : IR_Node
{
    int32_t value;

    IR_NODE_OVERRIDES;
};

struct IR_Single : IR_Node
{
    float value;
    
    IR_NODE_OVERRIDES;
};

struct IR_Double : IR_Node
{
    double value;

    IR_NODE_OVERRIDES;
};

struct IR_Array : IR_Node
{
    int32_t num_elements;
    struct IR_Type *element_type;
    IR_NODE_OVERRIDES;
};

struct IR_String : IR_Node
{
    std::string value;

    IR_NODE_OVERRIDES;
};

struct Callable_Parameter
{
    std::string name;
    IR_Type *type;
};

struct IR_Callable : IR_Node
{
    struct IR_Type *return_type;
    std::vector<Callable_Parameter> parameters;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_PRIMITIVES_HPP */
