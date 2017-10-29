#include "bound_function_map.hpp"

bool Bound_Function_Map::add_function(Primitive_Function *primitive)
{
    assert(primitive);
    auto n2p = m_bound_functions.find(primitive->name);
    if (n2p != m_bound_functions.end())
    {
        auto it = n2p->second.find(primitive->fn_type->parameter_types());
        if (it != n2p->second.end())
        {
            return false;
        }
        // subsequent
        m_bound_functions[primitive->name][primitive->fn_type->parameter_types()] =
            Bound_Function(primitive);
    }
    else
    {   // first
        m_bound_functions[primitive->name] = {
            {primitive->fn_type->parameter_types(), Bound_Function(primitive)}
        };
    }
    return true;
}

bool Bound_Function_Map::add_function(const std::string &name, Function_Type_Info *fn_type, IR_Label *code)
{
    assert(!name.empty());
    assert(fn_type);
    assert(code);
    auto n2p = m_bound_functions.find(name);
    if (n2p != m_bound_functions.end())
    {
        auto it = n2p->second.find(fn_type->parameter_types());
        if (it != n2p->second.end())
        {
            return false;
        }
        // subsequent
        m_bound_functions[name][fn_type->parameter_types()] = Bound_Function(fn_type, code);
    }
    else
    {   // first
        m_bound_functions[name] = {{fn_type->parameter_types(), Bound_Function(fn_type, code)}};
    }
    return true;
}

Bound_Function Bound_Function_Map::get_function(const std::string &name, const Function_Parameters &params)
{
    auto n2p = m_bound_functions.find(name);
    if (n2p != m_bound_functions.end())
    {
        auto it = n2p->second.find(params);
        if (it != n2p->second.end())
        {
            return it->second;
        }
    }
    return Bound_Function();
}

Bound_Functions Bound_Function_Map::get_functions(const std::string &name)
{
    Bound_Functions bfs;
    auto n2p = m_bound_functions.find(name);
    if (n2p != m_bound_functions.end())
    {
        for (auto &&f : n2p->second)
        {
            bfs.push_back(f.second);
        }
    }
    return bfs;
}

bool Bound_Function_Map::any(const std::string &name)
{
    return m_bound_functions.find(name) != m_bound_functions.end();
}
