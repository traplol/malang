#include "scope_lookup.hpp"

void Scope::show_parent()
{
    m_can_see_parent_scope = true;
}

void Scope::hide_parent()
{
    m_can_see_parent_scope = true;
}

Bound_Function_Map &Scope::bound_functions()
{
    return m_bound_function_map;
}

Symbol_Map &Scope::symbols()
{
    return m_symbol_map;
}

IR_Symbol *Scope::find_symbol(const std::string &name) const
{
    auto cur = this;
    while (cur)
    {
        if (auto sym = cur->m_symbol_map.get_symbol(name))
            return sym;
        if (!cur->m_can_see_parent_scope)
            break;
        cur = cur->m_parent;
    }
    return nullptr;
}

Bound_Function Scope::find_bound_function(const std::string &name, const Function_Parameters &param_types) const
{
    auto cur = this;
    while (cur)
    {
        auto bound_fn = cur->m_bound_function_map.get(name, param_types);
        if (bound_fn.is_valid())
            return bound_fn;
        if (!cur->m_can_see_parent_scope)
            break;
        cur = cur->m_parent;
    }
    return Bound_Function();
}

bool Scope::any(const std::string &name) const
{
    auto cur = this;
    while (cur)
    {
        if (cur->m_bound_function_map.any(name))
            return true;
        if (cur->m_symbol_map.any(name))
            return true;
        if (!cur->m_can_see_parent_scope)
            break;
        cur = cur->m_parent;
    }
    return false;
}

Scope_Lookup::~Scope_Lookup()
{
    auto scope = m_current_scope;
    while (scope)
    {
        auto next = scope->m_parent;
        delete scope;
        scope = next;
    }
}

Scope_Lookup::Scope_Lookup(Malang_IR *alloc)
    : m_alloc(alloc)
    , m_current_scope(new Scope{nullptr, alloc, 0, true})
{
    m_frame_stack.push_back(0);
}

void Scope_Lookup::push(bool entering_new_frame, bool can_see_parent_scope)
{
    auto scope = new Scope
        {
            m_current_scope,
            m_alloc,
            m_current_scope->symbols().index(),
            can_see_parent_scope
        };
    m_frame_stack.push_back(entering_new_frame);
    m_current_scope = scope;
}

void Scope_Lookup::pop()
{
    auto prev = m_current_scope->m_parent;
    assert(prev);

    if (m_frame_stack.back() == false)
    {
        prev->symbols().index(m_current_scope->symbols().index());
    }
    m_frame_stack.pop_back();

    delete m_current_scope;
    m_current_scope = prev;
}

Scope &Scope_Lookup::current()
{
    return *m_current_scope;
}

