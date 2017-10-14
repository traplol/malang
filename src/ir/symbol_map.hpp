#ifndef MALANG_IR_SYMBOL_MAP_HPP
#define MALANG_IR_SYMBOL_MAP_HPP

#include <map>

struct Symbol_Map
{
    Symbol_Map() : m_local_index(0) {}
    struct IR_Symbol *make_symbol(const std::string &name, struct IR_Type *type);
    struct IR_Symbol *get_symbol(const std::string &name);
private:
    size_t m_local_index;
    std::map<std::string, struct IR_Symbol*> m_map;
};

#endif /* MALANG_IR_SYMBOL_MAP_HPP */
