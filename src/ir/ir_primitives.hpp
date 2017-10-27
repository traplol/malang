#ifndef MALANG_IR_IR_PRIMITIVES_HPP
#define MALANG_IR_IR_PRIMITIVES_HPP

#include <vector>
#include <string>
#include <stdint.h>
#include "ir_values.hpp"

struct IR_Boolean : IR_RValue
{
    virtual ~IR_Boolean();
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
    virtual ~IR_Char();
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
    virtual ~IR_Fixnum();
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
    virtual ~IR_Single();
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
    virtual ~IR_Double();
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

struct IR_New_Array : IR_LValue
{
    virtual ~IR_New_Array();
    IR_New_Array(const Source_Location &src_loc, Array_Type_Info *type, Type_Token of_type, IR_Value *size)
        : IR_LValue(src_loc)
        , type(type)
        , size(size)
        , of_type(of_type)
        {}
    Array_Type_Info *type;
    IR_Value *size;
    Type_Token of_type;

    virtual Type_Info *get_type() const override { return type; }
    IR_NODE_OVERRIDES;
};

struct IR_String : IR_RValue
{
    virtual ~IR_String();
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
    virtual ~IR_Callable();
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

struct IR_Indexable : IR_LValue
{
    virtual ~IR_Indexable();
    IR_Indexable(const Source_Location &src_loc, IR_Value *thing, IR_Value *index, Type_Info *value_type)
        : IR_LValue(src_loc)
        , thing(thing)
        , index(index)
        , value_type(value_type)
        {}

    IR_Value *thing;
    IR_Value *index;
    Type_Info *value_type;
    virtual Type_Info *get_type() const override { return value_type; };

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_PRIMITIVES_HPP */
