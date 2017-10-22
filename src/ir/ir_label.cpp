#include "ir_label.hpp"
#include "../codegen/codegen.hpp"

IR_NODE_OVERRIDES_IMPL(IR_Label);
const std::string &IR_Label::name() const
{
    return m_name;
}
bool IR_Label::is_resolved() const
{
    return m_is_resolved;
}
int32_t IR_Label::address() const
{
    assert(is_resolved() && "using address when the label hasn't been resolved yet...");
    return m_local_address;
}
void IR_Label::address(int32_t addr)
{
    assert(!is_resolved() && "label address has already been resolved");
    m_local_address = addr;
    m_is_resolved = true;
    for (auto &&bf : m_to_be_backfilled)
    {
        if (bf.wants_relative)
        {
            bf.cg->set_raw_32(bf.index, m_local_address-bf.from);
        }
        else
        {
            bf.cg->set_raw_32(bf.index, m_local_address);
        }
    }
}
void IR_Label::please_backfill_on_resolve(struct Codegen *cg, size_t index)
{
    m_to_be_backfilled.push_back({cg, index, 0, false});
}
void IR_Label::please_backfill_on_resolve_rel(struct Codegen *cg, size_t index, size_t from)
{
    m_to_be_backfilled.push_back({cg, index, from, true});
}

IR_NODE_OVERRIDES_IMPL(IR_Named_Block);
std::vector<IR_Node*> &IR_Named_Block::body()
{
    return m_body;
}
IR_Label* IR_Named_Block::end() const
{
    return m_end;
}
