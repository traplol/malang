#ifndef MALANG_IR_SYMBOL_MAP_HPP
#define MALANG_IR_SYMBOL_MAP_HPP

#include <map>
#include <vector>
#include "../source_code.hpp"
#include "symbol_scope.hpp"

struct Malang_IR;
struct IR_Symbol;
struct Type_Info;

struct Symbol_Map
{
    Symbol_Map(Malang_IR *alloc)
        : m_alloc(alloc)
        , m_local_index(0)
        {}
    IR_Symbol *make_symbol(const std::string &name, Type_Info *type, const Source_Location &src_loc, Symbol_Scope scope);
    IR_Symbol *get_symbol(const std::string &name) const;
    bool any(const std::string &name) const;
    
private:
    Malang_IR *m_alloc;
    size_t m_local_index;
    std::map<std::string, IR_Symbol*> m_symbols;
};

#endif /* MALANG_IR_SYMBOL_MAP_HPP */
