#ifndef MALANG_IR_TYPE_MAP_HPP
#define MALANG_IR_TYPE_MAP_HPP

#include <map>

#include "vm/runtime/reflection.hpp"

struct Module;
struct Type_Map
{
    ~Type_Map();
    Type_Map();
    Type_Info *get_or_declare_type(const std::string &name);
    Type_Info *declare_type(const std::string &name, struct Type_Info *parent);
    Type_Info *declare_builtin_type(const std::string &name, Type_Info *parent, bool gc_managed);
    Function_Type_Info *declare_function(const Types &parameter_types, Type_Info *return_type, bool is_native);
    Array_Type_Info *get_array_type(Type_Info *of_type);

    Type_Info *get_type(const std::string &name);
    Type_Info *get_type(Type_Token type_token);

    Type_Info *get_void() const;
    Type_Info *get_int() const;
    Type_Info *get_char() const;
    Type_Info *get_double() const;
    Type_Info *get_string() const;
    Type_Info *get_bool() const;
    Type_Info *get_buffer() const;
    Module *module() const;
    void module(Module *mod);

    void dump() const;
private:
    Type_Info *declare_type(const std::string &name, struct Type_Info *parent, bool is_builtin, bool is_gc_managed);
    std::map<std::string, Type_Info*> m_types;
    std::map<std::string, Trait_Definition*> m_traits;
    std::vector<Type_Info*> m_types_fast;

    Module *m_module;
    Type_Info *m_void;
    Type_Info *m_int;
    Type_Info *m_char;
    Type_Info *m_double;
    Type_Info *m_string;
    Type_Info *m_bool;
    Type_Info *m_buffer;
};

#endif /* MALANG_IR_TYPE_MAP_HPP */
