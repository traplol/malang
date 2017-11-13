#ifndef MALANG_MODULE_MAP_HPP
#define MALANG_MODULE_MAP_HPP

#include <string>
#include <map>

struct Ast;
struct Module
{
    Module(const std::string &name)
        : m_name(name)
        {}
    const std::string &name() const { return m_name; }
private:
    std::string m_name;
};

struct Module_Map
{
    Module *get(const std::string &name);
private:
    std::map<std::string, Module*> m_modules;
};

#endif /* MALANG_MODULE_MAP_HPP */
