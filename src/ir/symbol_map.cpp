#include <cassert>
#include "ir_symbol.hpp"
#include "ir_type.hpp"
#include "symbol_map.hpp"

IR_Symbol *Symbol_Map::make_symbol(const std::string &name, IR_Type *type)
{
    assert(!name.empty());
    assert(type != nullptr);
    assert(get_symbol(name) == nullptr);

    auto sym = new IR_Symbol;
    sym->symbol = name;
    sym->type = type;
    sym->index = m_local_index++;
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
