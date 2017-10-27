#ifndef MALANG_IR_SYMBOL_MAP_HPP
#define MALANG_IR_SYMBOL_MAP_HPP

#include <map>
#include <vector>
#include "../source_code.hpp"
#include "symbol_scope.hpp"

struct Symbol_Map
{
    Symbol_Map(struct Malang_IR *alloc);
    ~Symbol_Map();
    struct IR_Symbol *make_symbol(const std::string &name, struct Type_Info *type, const Source_Location &src_loc, Symbol_Scope scope);
    struct IR_Symbol *get_symbol(const std::string &name);
    void reset_index();

    void push();
    void pop();
    
    using string_to_symbol_map = std::map<std::string, struct IR_Symbol*>;
private:
    size_t m_local_index;
    struct Malang_IR *m_alloc;
    string_to_symbol_map *m_map;
    std::vector<string_to_symbol_map*> m_levels;
};

#endif /* MALANG_IR_SYMBOL_MAP_HPP */
