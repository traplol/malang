#ifndef MALANG_IR_IR_SYMBOL_HPP
#define MALANG_IR_IR_SYMBOL_HPP

#include <string>
#include "ir.hpp"
#include "ir_values.hpp"
#include "symbol_scope.hpp"

struct IR_Symbol : IR_LValue
{
    virtual ~IR_Symbol() = default;
    IR_Symbol(const Source_Location &src_loc, const std::string &symbol, size_t index,
              Type_Info *type, bool is_readonly, bool is_private,
              Symbol_Scope scope = Symbol_Scope::Global, bool is_initialized = false)
        : IR_LValue(src_loc)
        , is_initialized(is_initialized)
        , is_readonly(is_readonly)
        , is_private(is_private)
        , scope(scope)
        , index(index)
        , type(type)
        , symbol(symbol)
        {}
    bool is_initialized;
    bool is_readonly;
    bool is_private;
    Symbol_Scope scope;
    size_t index;
    struct Type_Info *type;
    std::string symbol;
    virtual struct Type_Info *get_type() const override { return type; }

    IR_NODE_OVERRIDES;
};

#endif /* MALANG_IR_IR_SYMBOL_HPP */
