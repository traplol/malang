#include <cassert>
#include "ir_symbol.hpp"
#include "symbol_map.hpp"

Symbol_Map::Symbol_Map()
    : m_local_index(0)
    , m_map(new string_to_symbol_map)
{}

Symbol_Map::~Symbol_Map()
{
    delete m_map;
}

IR_Symbol *Symbol_Map::make_symbol(const std::string &name, Type_Info *type, const Source_Location &src_loc, Symbol_Scope scope)
{
    assert(!name.empty());
    assert(type != nullptr);
    assert(get_symbol(name) == nullptr);

    auto sym = new IR_Symbol{src_loc, name, m_local_index++, type, scope, false};
    (*m_map)[sym->symbol] = sym;
    return sym;
}

static inline
IR_Symbol *get_sym(Symbol_Map::string_to_symbol_map *m, const std::string &name)
{
    assert(m);
    auto it = m->find(name);
    if (it != m->end())
    {
        return it->second;
    }
    return nullptr;
}

IR_Symbol *Symbol_Map::get_symbol(const std::string &name)
{
    if (auto sym = get_sym(m_map, name))
    {
        return sym;
    }
    for (auto it = m_levels.rbegin(); it != m_levels.rend(); ++it)
    {
        if (auto sym = get_sym(*it, name))
        {
            return sym;
        }
    }
    return nullptr;
}

void Symbol_Map::reset_index()
{
    m_local_index = 0;
}

void Symbol_Map::push()
{
    m_levels.push_back(m_map);
    m_map = new string_to_symbol_map;
}
void Symbol_Map::pop()
{
    assert(!m_levels.empty());
    auto m = m_map;
    assert(m);
    m_map = m_levels.back();
    m_levels.pop_back();
    delete m;
}
