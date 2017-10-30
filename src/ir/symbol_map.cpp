#include <cassert>
#include "ir_symbol.hpp"
#include "symbol_map.hpp"
#include "ir.hpp"

IR_Symbol *Symbol_Map::make_symbol(const std::string &name, Type_Info *type, const Source_Location &src_loc, Symbol_Scope scope)
{
    assert(!name.empty());
    assert(type != nullptr);
    assert(get_symbol(name) == nullptr);

    auto sym = m_alloc->alloc<IR_Symbol>(src_loc, name, m_local_index++, type, scope, false);
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
