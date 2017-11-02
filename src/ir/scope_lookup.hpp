#ifndef MALANG_SCOPE_LOOKUP_HPP
#define MALANG_SCOPE_LOOKUP_HPP

#include "symbol_map.hpp"
#include "bound_function_map.hpp"

struct Scope
{
    Scope(Scope *parent, Malang_IR *alloc, size_t index_start, bool can_see_parent_scope)
        : m_parent(parent)
        , m_alloc(alloc)
        , m_can_see_parent_scope(can_see_parent_scope)
        , m_symbol_map(alloc, index_start)
        {}
    void show_parent();
    void hide_parent();
    bool any(const std::string &named) const;
    IR_Symbol *find_symbol(const std::string &name) const;
    Bound_Function find_bound_function(const std::string &name, const Function_Parameters &param_types) const;

    Bound_Function_Map &bound_functions();
    Symbol_Map &symbols();
private:
    friend struct Scope_Lookup;
    Scope *m_parent;
    Malang_IR *m_alloc;
    bool m_can_see_parent_scope;
    Symbol_Map m_symbol_map;
    Bound_Function_Map m_bound_function_map;
};

struct Scope_Lookup
{
    ~Scope_Lookup();
    Scope_Lookup(Malang_IR *alloc);
    void push(bool entering_new_frame, bool can_see_parent_scope = true);
    void pop();
    Scope &current();
private:
    std::vector<bool> m_frame_stack;
    Malang_IR *m_alloc;
    Scope *m_current_scope;
};

#endif /* MALANG_SCOPE_LOOKUP_HPP */
