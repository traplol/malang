#ifndef MALANG_IR_IR_PRIMITIVES_HPP
#define MALANG_IR_IR_PRIMITIVES_HPP

#include <vector>
#include <string>
#include <stdint.h>
#include "ir_values.hpp"

struct IR_Boolean : IR_RValue
{
    IR_Boolean(const Source_Location &src_loc, Type_Info *type, bool value)
        : IR_RValue(src_loc)
        , type(type)
        , value(value)
        {}
    Type_Info *type;
    bool value;

    virtual Type_Info *get_type() const override { return type; }
    
    IR_NODE_OVERRIDES;
};

struct IR_Char : IR_RValue
{
    IR_Char(const Source_Location &src_loc, Type_Info *type, Char value)
        : IR_RValue(src_loc)
        , type(type)
        , value(value)
        {}
    Type_Info *type;
    Char value;

    virtual Type_Info *get_type() const override { return type; }
    
    IR_NODE_OVERRIDES;
};

struct IR_Fixnum : IR_RValue
{
    IR_Fixnum(const Source_Location &src_loc, Type_Info *type, Fixnum value)
        : IR_RValue(src_loc)
        , type(type)
        , value(value)
        {}
    Type_Info *type;
    Fixnum value;

    virtual Type_Info *get_type() const override { return type; }

    IR_NODE_OVERRIDES;
};

struct IR_Single : IR_RValue
{
    IR_Single(const Source_Location &src_loc, Type_Info *type, float value)
        : IR_RValue(src_loc)
        , type(type)
        , value(value)
        {}
    Type_Info *type;
    float value;

    virtual Type_Info *get_type() const override { return type; }
    
    IR_NODE_OVERRIDES;
};

struct IR_Double : IR_RValue
{
    IR_Double(const Source_Location &src_loc, Type_Info *type, Double value)
        : IR_RValue(src_loc)
        , type(type)
        , value(value)
        {}
    Type_Info *type;
    Double value;

    virtual Type_Info *get_type() const override { return type; }

    IR_NODE_OVERRIDES;
};

struct IR_Array : IR_RValue
{
    IR_Array(const Source_Location &src_loc, Type_Info *type, int32_t num_elements)
        : IR_RValue(src_loc)
        , type(type)
        , num_elements(num_elements)
        {}
    Type_Info *type;
    int32_t num_elements;

    virtual Type_Info *get_type() const override { return type; }
    IR_NODE_OVERRIDES;
};

struct IR_String : IR_RValue
{
    IR_String(const Source_Location &src_loc, Type_Info *type, const std::string &value)
        : IR_RValue(src_loc)
        , type(type)
        , value(value)
        {}
    Type_Info *type;
    std::string value;

    virtual Type_Info *get_type() const override { return type; }
    IR_NODE_OVERRIDES;
};

struct IR_Callable : IR_RValue
{
    IR_Callable(const Source_Location &src_loc, struct IR_Label *label, struct Function_Type_Info *fn_type)
        : IR_RValue(src_loc)
        , fn_type(fn_type)
        {
            u.label = label;
        }
    IR_Callable(const Source_Location &src_loc, Fixnum index, struct Function_Type_Info *fn_type)
        : IR_RValue(src_loc)
        , fn_type(fn_type)
        {
            u.index = index;
        }

    union {
        struct IR_Label *label;
        Fixnum index;
    } u;
    struct Function_Type_Info *fn_type;
    virtual Type_Info *get_type() const override { return fn_type; }

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_PRIMITIVES_HPP */
