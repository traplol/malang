#ifndef MALANG_MODULE_MAP_HPP
#define MALANG_MODULE_MAP_HPP

#include <string>
#include <vector>
#include <map>

struct Module_Map;
struct Module
{
    const std::string &name() const;
    const std::string &filepath();
    const std::string &fully_qualified_name();
    Module_Map *module_map();
    Module *parent();
    Module *find_child(const std::vector<std::string> &name);
    void add_child(Module *mod);
private:
    friend Module_Map;
    Module(const std::string &name)
        : m_parent(nullptr)
        , m_name(name)
        {}
    std::string build_name(const std::string &sep) const;
    using name_itr = std::vector<std::string>::const_iterator;
    Module *find_child(name_itr &beg, name_itr &end);
    Module_Map *m_mod_map;
    Module *m_parent;
    std::string m_name;
    std::string m_filepath;
    std::string m_fully_qualified_name;
    std::map<std::string, Module*> m_children;
};

struct Module_Map
{
    ~Module_Map();
    Module_Map();
    Module *get(const std::vector<std::string> &name);
private:
    Module *make_mod(const std::string &name);
    std::vector<Module*> m_all_modules;
    std::vector<std::string> m_search_directories;
    std::map<std::string, Module*> m_root_modules;
};

#endif /* MALANG_MODULE_MAP_HPP */
