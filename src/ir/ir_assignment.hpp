#ifndef MALANG_IR_IR_ASSIGNMENT
#define MALANG_IR_IR_ASSIGNMENT

#include "ir.hpp"
#include "symbol_scope.hpp"

struct IR_Assignment : IR_Node
{
    IR_Assignment(const Source_Location &src_loc, struct IR_LValue *lhs, struct IR_Value *rhs, Symbol_Scope scope)
        : IR_Node(src_loc)
        , lhs(lhs)
        , rhs(rhs)
        , scope(scope)
        {}
    struct IR_LValue *lhs;
    struct IR_Value *rhs;
    Symbol_Scope scope;

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_ASSIGNMENT */
