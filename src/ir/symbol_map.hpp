#ifndef MALANG_IR_SYMBOL_MAP_HPP
#define MALANG_IR_SYMBOL_MAP_HPP

#include <map>

#include "ir_symbol.hpp"
#include "ir_type.hpp"

struct Symbol_Map
{
    Symbol_Map() : m_local_index(0) {}
    IR_Symbol *make_symbol(const std::string &name, IR_Type *type);
    IR_Symbol *get_symbol(const std::string &name);
private:
    size_t m_local_index;
    std::map<std::string, IR_Symbol*> m_map;
};

#endif /* MALANG_IR_SYMBOL_MAP_HPP */
