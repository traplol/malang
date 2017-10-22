#include <cassert>
#include "ir_symbol.hpp"
#include "symbol_map.hpp"

IR_Symbol *Symbol_Map::make_symbol(const std::string &name, Type_Info *type, const Source_Location &src_loc, Symbol_Scope scope)
{
    assert(!name.empty());
    assert(type != nullptr);
    assert(get_symbol(name) == nullptr);

    auto sym = new IR_Symbol{src_loc, name, m_local_index++, type, scope, false};
    m_map[sym->symbol] = sym;
    return sym;
}

IR_Symbol *Symbol_Map::get_symbol(const std::string &name)
{
    auto it = m_map.find(name);
    if (it != m_map.end())
    {
        return it->second;
    }
    return nullptr;
}
