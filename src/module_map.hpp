#ifndef MALANG_MODULE_MAP_HPP
#define MALANG_MODULE_MAP_HPP

#include <string>
#include <vector>
#include <map>

struct Module_Map;
struct Scope_Lookup;
struct Malang_IR;

struct Module
{
    ~Module();

    constexpr static int white = 0;
    constexpr static int grey = 1;
    constexpr static int black = 2;
    const std::string &name() const;
    const std::string &filepath();
    const std::string &abspath();
    const std::string &fully_qualified_name();
    Module_Map *module_map();
    Module *parent();
    Module *find_child(const std::vector<std::string> &name);
    void add_child(Module *mod);
    int color() const;
    void color(int c);
    Scope_Lookup *locality();
private:
    friend Module_Map;
    Module(const std::string &name)
        : m_parent(nullptr)
        , m_locality(nullptr)
        , m_name(name)
        {}
    std::string build_name(const std::string &sep) const;
    using name_itr = std::vector<std::string>::const_iterator;
    int m_color;
    Module *find_child(name_itr &beg, name_itr &end);
    Module_Map *m_mod_map;
    Module *m_parent;
    Scope_Lookup *m_locality;
    std::string m_name;
    std::string m_filepath;
    std::string m_abspath;
    std::string m_fully_qualified_name;
    std::map<std::string, Module*> m_children;
};

struct Module_Map
{
    ~Module_Map();
    Module_Map(Malang_IR *alloc);
    Module *get(const std::vector<std::string> &name);
    Module *get_existing(const std::vector<std::string> &name);
    bool find_file(Module *module, std::string &filename);
    bool find_file_rel(const std::string &rel_path, Module *module, std::string &filename);
    void add_search_directory(const std::string &dir);
private:
    Module *get_or_make_mod(const std::vector<std::string> &name, bool can_make);
    Module *make_mod(const std::string &name);
    Malang_IR *m_alloc;
    std::vector<Module*> m_all_modules;
    std::vector<std::string> m_search_directories;
    std::map<std::string, Module*> m_root_modules;
};

#endif /* MALANG_MODULE_MAP_HPP */
