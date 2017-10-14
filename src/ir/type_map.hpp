#ifndef MALANG_IR_TYPE_MAP_HPP
#define MALANG_IR_TYPE_MAP_HPP

#include <map>

struct Type_Map
{
    Type_Map() : m_index(0) {}
    struct IR_Type *make_type(const std::string &name, struct IR_Type *parent);
    struct IR_Type *get_type(const std::string &name);
private:
    size_t m_index;
    std::map<std::string, struct IR_Type*> m_map;
};

#endif /* MALANG_IR_TYPE_MAP_HPP */
