#ifndef MALANG_IR_IR_ASSIGNMENT
#define MALANG_IR_IR_ASSIGNMENT

#include "ir.hpp"
#include "ir_values.hpp"
#include "symbol_scope.hpp"

struct IR_Assignment : IR_Node
{
    virtual ~IR_Assignment();
    IR_Assignment(const Source_Location &src_loc, IR_LValue *lhs, IR_Value *rhs, Symbol_Scope scope)
        : IR_Node(src_loc)
        , lhs(lhs)
        , rhs(rhs)
        , scope(scope)
        {}
    IR_LValue *lhs;
    IR_Value *rhs;
    Symbol_Scope scope;

    IR_NODE_OVERRIDES;
};

struct IR_Assign_Top : IR_Node
{
    virtual ~IR_Assign_Top();
    IR_Assign_Top(const Source_Location &src_loc, IR_LValue *lhs, Symbol_Scope scope)
        : IR_Node(src_loc)
        , lhs(lhs)
        , scope(scope)
        {}
    IR_LValue *lhs;
    Symbol_Scope scope;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_ASSIGNMENT */
