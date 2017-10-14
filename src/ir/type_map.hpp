#ifndef MALANG_IR_TYPE_MAP_HPP
#define MALANG_IR_TYPE_MAP_HPP

#include <map>

#include "ir_type.hpp"

struct Type_Map
{
    Type_Map() : m_index(0) {}
    IR_Type *make_type(const std::string &name, IR_Type *parent);
    IR_Type *get_type(const std::string &name);
private:
    size_t m_index;
    std::map<std::string, IR_Type*> m_map;
};

#endif /* MALANG_IR_TYPE_MAP_HPP */
