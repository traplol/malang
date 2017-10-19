#include <cassert>
#include "ir_label.hpp"
#include "label_map.hpp"

IR_Label *Label_Map::make_label(const std::string &name, const Source_Location &src_loc)
{
    assert(!name.empty());
    assert(get_label(name) == nullptr);

    auto label = new IR_Label{src_loc};
    label->name = name;
    label->resolved = false;
    m_map[label->name] = label;
    return label;
}

IR_Named_Block *Label_Map::make_named_block(const std::string &name, const std::string &end_name, const Source_Location &src_loc)
{
    assert(!name.empty());
    assert(!end_name.empty());
    assert(name != end_name);
    assert(get_label(name) == nullptr);
    assert(get_label(end_name) == nullptr);

    auto block = new IR_Named_Block{src_loc};
    block->name = name;
    block->resolved = false;
    auto end = make_label(end_name, src_loc);
    block->end = end;
    m_map[block->name] = block;
    return block;
}

IR_Label *Label_Map::get_label(const std::string &name)
{
    auto it = m_map.find(name);
    if (it != m_map.end())
    {
        return it->second;
    }
    return nullptr;
}

IR_Named_Block *Label_Map::get_named_block(const std::string &name)
{
    auto block = get_label(name);
    return dynamic_cast<IR_Named_Block*>(block);
}
