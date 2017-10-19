#ifndef MALANG_IR_IR_PRIMITIVES_HPP
#define MALANG_IR_IR_PRIMITIVES_HPP

#include <vector>
#include <string>
#include <stdint.h>
#include "ir_values.hpp"

struct IR_Boolean : IR_RValue
{
    IR_Boolean(const Source_Location &src_loc, struct Type_Info *type)
        : IR_RValue(src_loc)
        , type(type)
        {}
    struct Type_Info *type;
    bool value;

    virtual struct Type_Info *get_type() const override { return type; }
    
    IR_NODE_OVERRIDES;
};

struct IR_Char : IR_RValue
{
    IR_Char(const Source_Location &src_loc, struct Type_Info *type)
        : IR_RValue(src_loc)
        , type(type)
        {}
    struct Type_Info *type;
    char value;

    virtual struct Type_Info *get_type() const override { return type; }
    
    IR_NODE_OVERRIDES;
};

struct IR_Fixnum : IR_RValue
{
    IR_Fixnum(const Source_Location &src_loc, struct Type_Info *type)
        : IR_RValue(src_loc)
        , type(type)
        {}
    struct Type_Info *type;
    int32_t value;

    virtual struct Type_Info *get_type() const override { return type; }

    IR_NODE_OVERRIDES;
};

struct IR_Single : IR_RValue
{
    IR_Single(const Source_Location &src_loc, struct Type_Info *type)
        : IR_RValue(src_loc)
        , type(type)
        {}
    struct Type_Info *type;
    float value;

    virtual struct Type_Info *get_type() const override { return type; }
    
    IR_NODE_OVERRIDES;
};

struct IR_Double : IR_RValue
{
    IR_Double(const Source_Location &src_loc, struct Type_Info *type)
        : IR_RValue(src_loc)
        , type(type)
        {}
    struct Type_Info *type;
    double value;

    virtual struct Type_Info *get_type() const override { return type; }

    IR_NODE_OVERRIDES;
};

struct IR_Array : IR_RValue
{
    IR_Array(const Source_Location &src_loc, struct Type_Info *type)
        : IR_RValue(src_loc)
        , type(type)
        {}
    struct Type_Info *type;
    int32_t num_elements;

    virtual struct Type_Info *get_type() const override { return type; }
    IR_NODE_OVERRIDES;
};

struct IR_String : IR_RValue
{
    IR_String(const Source_Location &src_loc, struct Type_Info *type)
        : IR_RValue(src_loc)
        , type(type)
        {}
    struct Type_Info *type;
    std::string value;

    virtual struct Type_Info *get_type() const override { return type; }
    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_PRIMITIVES_HPP */
