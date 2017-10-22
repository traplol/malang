#ifndef MALANG_IR_IR_SYMBOL_HPP
#define MALANG_IR_IR_SYMBOL_HPP

#include <string>
#include "ir.hpp"
#include "ir_values.hpp"
#include "symbol_scope.hpp"

struct IR_Symbol : IR_LValue
{
    IR_Symbol(const Source_Location &src_loc, const std::string &symbol, size_t index,
              Type_Info *type, Symbol_Scope scope = Symbol_Scope::Global, bool is_initialized = false)
        : IR_LValue(src_loc)
        , symbol(symbol)
        , index(index)
        , type(type)
        , scope(scope)
        , is_initialized(is_initialized)
        {}
    std::string symbol;
    size_t index;
    struct Type_Info *type;
    Symbol_Scope scope;
    bool is_initialized;
    virtual struct Type_Info *get_type() const override { return type; }

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_SYMBOL_HPP */
