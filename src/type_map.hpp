#ifndef MALANG_IR_TYPE_MAP_HPP
#define MALANG_IR_TYPE_MAP_HPP

#include <map>

#include "vm/reflection.hpp"

struct Type_Map
{
    Type_Info *get_or_declare_type(const std::string &name);
    Type_Info *declare_type(const std::string &name, struct Type_Info *parent);
    Function_Type_Info *declare_function(Type_Info *return_type, const std::vector<Type_Info*> parameter_types);
    Type_Info *get_type(const std::string &name);
    Type_Info *get_type(Type_Token type_token);
private:
    std::map<std::string, Type_Info*> m_types;
    std::vector<Type_Info*> m_types_fast;
};

#endif /* MALANG_IR_TYPE_MAP_HPP */
