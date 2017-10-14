#include <cassert>
#include "ir_type.hpp"
#include "type_map.hpp"

IR_Type *Type_Map::make_type(const std::string &name, IR_Type *parent)
{
    assert(!name.empty());
    assert(get_type(name) == nullptr);

    auto type = new IR_Type;
    type->parent = parent;
    type->name = name;
    type->index = m_index++;
    m_map[type->name] = type;
    return type;
}

IR_Type *Type_Map::get_type(const std::string &name)
{
    auto it = m_map.find(name);
    if (it != m_map.end())
    {
        return it->second;
    }
    return nullptr;
}

