#include <cassert>
#include <sstream>
#include <unistd.h> // getcwd: @FixMe: move this to OS agnostic utility
#include "module_map.hpp"

const std::string &Module::name() const
{
    return m_name;
}

Module *Module::parent()
{
    return m_parent;
}

Module_Map *Module::module_map()
{
    return m_mod_map;
}

void Module::add_child(Module *module)
{
    assert(module);
    assert(!module->m_parent);
    assert(!module->m_name.empty());

    auto exists = m_children.find(module->m_name);
    if (exists != m_children.end())
    {
        printf("module name conflict: `%s:%s' already exists\n",
               fully_qualified_name().c_str(), module->name().c_str());
        abort();
    }
    module->m_parent = this;
    m_children[module->m_name] = module;
}

Module *Module::find_child(name_itr &beg, name_itr &end)
{
    auto it = m_children.find(*beg);
    if (it == m_children.end())
    {
        return nullptr;
    }

    if (beg == end)
    {
        return it->second;
    }
    beg++;
    return it->second->find_child(beg, end);
}

Module *Module::find_child(const std::vector<std::string> &name)
{
    name_itr beg = name.begin();
    name_itr end = name.end();
    auto m = find_child(beg, end);
    if (m)
    {
        assert(beg == end);
    }
    return m;
}

const std::string &Module::filepath()
{
    if (!m_filepath.empty())
    {
        return m_filepath;
    }

    m_filepath = build_name("/") + ".ma";
    return m_filepath;
}

const std::string &Module::fully_qualified_name()
{
    if (!m_fully_qualified_name.empty())
    {
        return m_fully_qualified_name;
    }

    m_fully_qualified_name = build_name("$");
    return m_fully_qualified_name;
}

std::string Module::build_name(const std::string &sep) const
{
    if (m_parent)
    {
        return m_parent->build_name(sep) + sep + m_name;
    }
    return m_name;
}

Module *Module_Map::get(const std::vector<std::string> &name)
{
    assert(!name.empty());
    auto it = m_root_modules.find(name[0]);
    if (it != m_root_modules.end())
    {
        if (name.size() == 1)
        {
            return it->second;
        }

        auto child = it->second->find_child({name[1]});
        if (!child)
        {
            auto mod = make_mod(name[1]);
            m_all_modules.push_back(mod);
            it->second->add_child(mod);
            child = mod;
        }
        for (size_t i = 2; i < name.size(); ++i)
        {
            if (auto tmp = child->find_child({name[i]}))
            {
                child = tmp;
            }
            else
            {
                auto mod = make_mod(name[i]);
                child->add_child(mod);
                child = mod;
            }
        }
        return child;
    }
    else
    {   // If the root node doesn't exist then none of the subnodes exist either
        auto root = make_mod(name[0]);
        auto cur = root;
        for (size_t i = 1; i < name.size(); ++i)
        {
            auto tmp = make_mod(name[i]);
            cur->add_child(tmp);
            cur = tmp;
        }
        m_root_modules[name[0]]= root;
        return cur;
    }
}

Module *Module_Map::make_mod(const std::string &name)
{
    auto mod = new Module(name);
    mod->m_mod_map = this;
    m_all_modules.push_back(mod);
    return mod;
}

Module_Map::~Module_Map()
{
    for (auto m : m_all_modules)
    {
        delete m;
    }
    m_all_modules.clear();
    m_root_modules.clear();
    m_search_directories.clear();
}

Module_Map::Module_Map()
{
    char buf[2048]{};
    if (getcwd(buf, 2048))
    {
        m_search_directories.push_back(buf);
    }
    else
    {
        m_search_directories.push_back("");
    }
}
