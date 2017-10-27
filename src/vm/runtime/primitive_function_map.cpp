#include <stdio.h>
#include "primitive_function_map.hpp"

Primitive_Function_Map::~Primitive_Function_Map()
{
    for (auto &&kvp : m_builtins)
    {
        delete kvp.second;
    }
    m_builtins.clear();
    m_primitives.clear();
}

Primitive_Function *Primitive_Function_Map::add_builtin(const std::string &name, struct Function_Type_Info *fn_type, Native_Code native_code)
{
    auto it = m_builtins.find(name);
    if (it != m_builtins.end())
    {
        printf("builtin: `%s' already defined\n", name.c_str());
        abort();
    }
    auto pfn = add_primitive(fn_type, native_code);

    m_builtins[name] = pfn;
    return pfn;
}

Primitive_Function *Primitive_Function_Map::get_builtin(const std::string &name)
{
    auto it = m_builtins.find(name);
    if (it != m_builtins.end())
    {
        return it->second;
    }
    return nullptr;
}

Primitive_Function *Primitive_Function_Map::add_primitive(struct Function_Type_Info *fn_type, Native_Code native_code)
{
    auto pfn = new Primitive_Function{static_cast<Fixnum>(m_primitives.size()), native_code, fn_type};
    m_primitives.push_back(pfn->native_code);
    return pfn;
}

std::vector<Native_Code> Primitive_Function_Map::primitives() const
{
    return m_primitives;
}
