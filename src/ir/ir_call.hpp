#ifndef MALANG_IR_IR_CALL_HPP
#define MALANG_IR_IR_CALL_HPP

#include <vector>
#include "ir.hpp"
#include "ir_values.hpp"

struct IR_Call : IR_RValue
{
    virtual ~IR_Call();
    IR_Call(const Source_Location &src_loc, struct IR_Value *callee, const std::vector<IR_Value*> &arguments)
        : IR_RValue(src_loc)
        , callee(callee)
        , arguments(std::move(arguments))
        {}
    virtual struct Type_Info *get_type() const override;
    struct Function_Type_Info *get_fn_type() const;
    IR_Value *callee;
    std::vector<IR_Value*> arguments;

    IR_NODE_OVERRIDES;
};

struct IR_Call_Method : IR_Call
{
    virtual ~IR_Call_Method() = default;
    IR_Call_Method(const Source_Location &src_loc, struct IR_Value *callee, const std::vector<IR_Value*> &arguments)
        : IR_Call(src_loc, callee, arguments)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_Call_Virtual_Method : IR_Call
{
    virtual ~IR_Call_Virtual_Method() = default;
    IR_Call_Virtual_Method(const Source_Location &src_loc, struct IR_Value *callee, const std::vector<IR_Value*> &arguments)
        : IR_Call(src_loc, callee, arguments)
        {}
    IR_NODE_OVERRIDES;
};

struct IR_Allocate_Locals : IR_Node
{
    virtual ~IR_Allocate_Locals() = default;
    IR_Allocate_Locals(const Source_Location &src_loc, uint16_t num_to_alloc)
        : IR_Node(src_loc)
        , num_to_alloc(num_to_alloc)
        {}

    uint16_t num_to_alloc;
    IR_NODE_OVERRIDES;
};

struct IR_Callable : IR_RValue
{
    virtual ~IR_Callable();
    IR_Callable(const Source_Location &src_loc, IR_Label *label, Function_Type_Info *fn_type, bool is_special_bound = false)
        : IR_RValue(src_loc)
        , fn_type(fn_type)
        , is_special_bound(is_special_bound)
        {
            u.label = label;
        }
    IR_Callable(const Source_Location &src_loc, Fixnum index, Function_Type_Info *fn_type)
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
    bool is_special_bound;
    virtual Type_Info *get_type() const override { return fn_type; }

    IR_NODE_OVERRIDES;
};

struct IR_Method: IR_Callable
{
    virtual ~IR_Method();
    IR_Method(const Source_Location &src_loc, IR_Value *thing, IR_Label *label, struct Function_Type_Info *fn_type, bool is_special_bound = false)
        : IR_Callable(src_loc, label, fn_type, is_special_bound)
        , thing(thing)
        {}
        
    IR_Method(const Source_Location &src_loc, IR_Value *thing, Fixnum index, struct Function_Type_Info *fn_type)
        : IR_Callable(src_loc, index, fn_type)
        , thing(thing)
        {}
    IR_Value *thing;
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

#endif /* MALANG_IR_IR_CALL_HPP */
