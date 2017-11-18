#include "bound_function_map.hpp"

Bound_Function_Map::~Bound_Function_Map()
{
    // TODO: Need to free bound functions too
    for (auto &&fn : m_natives_to_free)
    {
        delete fn;
    }
}

bool Bound_Function_Map::add_constructor(Type_Info *to_type, Function_Type_Info *fn_type, Native_Code native)
{
    assert(to_type);
    assert(fn_type);
    assert(native);
    auto index = static_cast<Fixnum>(m_all_natives.size());
    auto ctor_name = to_type->name() + "..ctor";
    auto pfn = new Native_Function{ctor_name, index, native, fn_type};
    auto ctor = new Constructor_Info{fn_type, pfn};
    if (!to_type->add_constructor(ctor))
    {
        printf("Couldn't add constructor to %s\n", to_type->name().c_str());
        delete ctor;
        abort();
        //return false;
    }
    m_all_natives.push_back(native);
    return true;
}

bool Bound_Function_Map::declare_method(Type_Info *to_type, const std::string &name, Function_Type_Info *fn_type)
{
    assert(to_type);
    assert(!name.empty());
    assert(fn_type);

    auto method = new Method_Info{name, fn_type};
    if (!to_type->add_method(method))
    {
        printf("Couldn't add method %s to %s\n", name.c_str(), to_type->name().c_str());
        delete method;
        abort();
        //return false;
    }
    return true;
}
bool Bound_Function_Map::add_method(Type_Info *to_type, const std::string &name, Function_Type_Info *fn_type, Native_Code native)
{
    assert(to_type);
    assert(!name.empty());
    assert(fn_type);
    assert(native);
    auto index = static_cast<Fixnum>(m_all_natives.size());
    auto pfn = new Native_Function{name, index, native, fn_type};
    auto method = new Method_Info{name, fn_type, pfn};
    if (!to_type->add_method(method))
    {
        printf("Couldn't add method %s to %s\n", name.c_str(), to_type->name().c_str());
        delete method;
        abort();
        //return false;
    }
    m_all_natives.push_back(native);
    return true;
}

bool Bound_Function_Map::add_method(Type_Info *to_type, const std::string &name, Function_Type_Info *fn_type, IR_Label *code)
{
    assert(to_type);
    assert(!name.empty());
    assert(fn_type);
    assert(code);

    auto method = new Method_Info{name, fn_type, code};
    if (!to_type->add_method(method))
    {
        printf("Couldn't add method %s to %s\n", name.c_str(), to_type->name().c_str());
        delete method;
        abort();
        //return false;
    }
    return true;
}


bool Bound_Function_Map::add(const std::string &name, Function_Type_Info *fn_type, Native_Code native)
{
    auto n2p = m_free_functions.find(name);
    if (n2p != m_free_functions.end())
    {
        auto it = n2p->second.find(fn_type->parameter_types());
        if (it != n2p->second.end())
        {
            return false;
        }
        // subsequent
        auto index = static_cast<Fixnum>(m_all_natives.size());
        auto pfn = new Native_Function{name, index, native, fn_type};
        m_free_functions[name][fn_type->parameter_types()] = new Bound_Function(pfn);
        m_all_natives.push_back(native);
        m_natives_to_free.push_back(pfn);
    }
    else
    {   // first
        auto index = static_cast<Fixnum>(m_all_natives.size());
        auto pfn = new Native_Function{name, index, native, fn_type};
        m_free_functions[name] = {{fn_type->parameter_types(), new Bound_Function(pfn)}};
        m_all_natives.push_back(native);
        m_natives_to_free.push_back(pfn);
    }
    return true;
}

bool Bound_Function_Map::add(const std::string &name, Function_Type_Info *fn_type, IR_Label *code)
{
    assert(!name.empty());
    assert(fn_type);
    assert(code);
    auto n2p = m_free_functions.find(name);
    if (n2p != m_free_functions.end())
    {
        auto it = n2p->second.find(fn_type->parameter_types());
        if (it != n2p->second.end())
        {
            return false;
        }
        // subsequent
        m_free_functions[name][fn_type->parameter_types()] = new Bound_Function(fn_type, code);
    }
    else
    {   // first
        m_free_functions[name] = {{fn_type->parameter_types(), new Bound_Function(fn_type, code)}};
    }
    return true;
}

Bound_Function *Bound_Function_Map::get(const std::string &name, const Function_Parameters &params) const
{
    auto n2p = m_free_functions.find(name);
    if (n2p != m_free_functions.end())
    {
        auto it = n2p->second.find(params);
        if (it != n2p->second.end())
        {
            return it->second;
        }
    }
    return nullptr;
}

Bound_Functions Bound_Function_Map::get(const std::string &name) const
{
    Bound_Functions bfs;
    auto n2p = m_free_functions.find(name);
    if (n2p != m_free_functions.end())
    {
        for (auto &&f : n2p->second)
        {
            bfs.push_back(f.second);
        }
    }
    return bfs;
}

std::vector<Native_Code> Bound_Function_Map::natives() const
{
    return m_all_natives;
}

bool Bound_Function_Map::any(const std::string &name) const
{
    return m_free_functions.find(name) != m_free_functions.end();
}

void Bound_Function_Map::dump() const
{
    for (auto &&kvp : m_free_functions)
    {
        for (auto &&kvp2 : kvp.second)
        {
            printf("fn %s %s\n", kvp.first.c_str(), kvp2.first.to_string().c_str());
        }
    }
}
