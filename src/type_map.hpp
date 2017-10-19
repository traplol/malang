#ifndef MALANG_IR_TYPE_MAP_HPP
#define MALANG_IR_TYPE_MAP_HPP

#include <map>

#include "vm/reflection.hpp"

struct Type_Map
{
    Type_Map();
    Type_Info *get_or_declare_type(const std::string &name);
    Type_Info *declare_type(const std::string &name, struct Type_Info *parent);
    Function_Type_Info *declare_function(const std::vector<Type_Info*> &parameter_types, Type_Info *return_type);
    Type_Info *get_type(const std::string &name);
    Type_Info *get_type(Type_Token type_token);

    Type_Info *get_void() const;
    Type_Info *get_object() const;
    Type_Info *get_int() const;
    //Type_Info *get_char() const;
    Type_Info *get_double() const;
    Type_Info *get_string() const;
    Type_Info *get_bool() const;
private:
    std::map<std::string, Type_Info*> m_types;
    std::vector<Type_Info*> m_types_fast;

    Type_Info *m_void;
    Type_Info *m_object;
    Type_Info *m_int;
    Type_Info *m_char;
    Type_Info *m_double;
    Type_Info *m_string;
    Type_Info *m_bool;
};

#endif /* MALANG_IR_TYPE_MAP_HPP */
