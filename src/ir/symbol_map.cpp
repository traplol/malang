#include <cassert>
#include "ir_symbol.hpp"
#include "symbol_map.hpp"
#include "ir.hpp"

IR_Symbol *Symbol_Map::make_symbol(const Source_Location &src_loc, const std::string &name, Type_Info *type, bool is_readonly, Symbol_Scope scope)
{
    assert(!name.empty());
    assert(type != nullptr);
    assert(get_symbol(name) == nullptr);

    auto sym = m_alloc->alloc<IR_Symbol>(src_loc, name, m_local_index++, type, is_readonly, false, scope, false);
    m_symbols[sym->symbol] = sym;
    return sym;
}

IR_Symbol *Symbol_Map::get_symbol(const std::string &name) const
{
    auto it = m_symbols.find(name);
    if (it != m_symbols.end())
        return it->second;
    return nullptr;
}

bool Symbol_Map::any(const std::string &name) const
{
    return get_symbol(name) != nullptr;
}

size_t Symbol_Map::index() const
{
    return m_local_index;
}

void Symbol_Map::index(size_t i)
{
    m_local_index = i;
}

void Symbol_Map::dump() const
{
    for (auto &&kvp : m_symbols)
    {
        auto t = kvp.second->get_type();
        printf("%s: %s @ %i\n",
               kvp.first.c_str(),
               t ? t->name().c_str() : "null",
               (int)kvp.second->index);
    }
}
