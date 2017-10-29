#include <stdio.h>
#include "primitive_function_map.hpp"

Primitive_Function_Map::~Primitive_Function_Map()
{
    for (auto &&kvp : m_builtins)
    {
        for (auto &&kvp2 : kvp.second)
        {
            delete kvp2.second;
        }
    }
    m_builtins.clear();
    m_primitives.clear();
}

Primitive_Function *Primitive_Function_Map::add_builtin(const std::string &name, Function_Type_Info *fn_type, Native_Code native_code)
{
    auto n2p = m_builtins.find(name);
    if (n2p != m_builtins.end())
    {
        auto p2f = n2p->second.find(fn_type->parameter_types());
        if (p2f != n2p->second.end())
        {
            printf("builtin: `%s' :: %s already defined\n",
                   name.c_str(), fn_type->name().c_str());
            abort();
        }
        // Subsequent
        auto pfn = add_primitive(name, fn_type, native_code);
        m_builtins[name][fn_type->parameter_types()] = pfn;
        return pfn;
    }
    else
    {   // First
        auto pfn = add_primitive(name, fn_type, native_code);
        m_builtins[name] = {{fn_type->parameter_types(), pfn}};
        return pfn;
    }
}

Primitive_Function *Primitive_Function_Map::get_builtin(const std::string &name, const Function_Parameters &params)
{
    auto n2p = m_builtins.find(name);
    if (n2p != m_builtins.end())
    {
        auto p2f = n2p->second.find(params);
        if (p2f != n2p->second.end())
        {
            return p2f->second;
        }
    }
    return nullptr;
}

std::vector<Primitive_Function*> Primitive_Function_Map::get_builtins(const std::string &name)
{
    std::vector<Primitive_Function*> res;
    auto n2p = m_builtins.find(name);
    if (n2p != m_builtins.end())
    {
        for (auto &&p2f : n2p->second)
        {
            res.push_back(p2f.second);
        }
    }
    return res;
}

bool Primitive_Function_Map::builtin_exists(const std::string &name)
{
    auto n2p = m_builtins.find(name);
    if (n2p != m_builtins.end())
    {
        return true;
    }
    return false;
}

Primitive_Function *Primitive_Function_Map::add_primitive(const std::string &name, struct Function_Type_Info *fn_type, Native_Code native_code)
{
    auto pfn = new Primitive_Function{name,
                                      static_cast<Fixnum>(m_primitives.size()),
                                      native_code, fn_type};
    m_primitives.push_back(pfn->native_code);
    return pfn;
}

std::vector<Native_Code> Primitive_Function_Map::primitives() const
{
    return m_primitives;
}

void Primitive_Function_Map::dump() const
{
    for (auto &&n2p : m_builtins)
    {
        auto name = n2p.first.c_str();
        for (auto &&p2f : n2p.second)
        {
            printf("%s :: %s\n", name, p2f.second->fn_type->name().c_str());
        }
    }
}
