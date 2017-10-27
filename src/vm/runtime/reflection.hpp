#ifndef MALANG_VM_REFLECTION_HPP
#define MALANG_VM_REFLECTION_HPP

#include <string>
#include <vector>
#include "primitive_types.hpp"

struct Function_Type_Info;
struct Array_Type_Info;
struct Type_Info;
using Type_Token = Fixnum;

struct Method_Info
{
    ~Method_Info();
    Method_Info(const std::string &name, Function_Type_Info *fn_type)
        : m_name(name)
        , m_fn_type(fn_type)
        , m_is_native(false)
        {}

    Method_Info(const std::string &name, Function_Type_Info *fn_type, Primitive_Function *prim)
        : m_name(name)
        , m_fn_type(fn_type)
        {
            set_function(prim);
        }

    Method_Info(const std::string &name, Function_Type_Info *fn_type, int32_t code_ip)
        : m_name(name)
        , m_fn_type(fn_type)
        {
            set_function(code_ip);
        }

    Function_Type_Info *type() const;
    const std::string &name() const;
    const std::vector<Type_Info*> &parameter_types() const;
    Type_Info *return_type() const;

    void set_function(Primitive_Function *prim);
    void set_function(int32_t code_ip);
    bool is_native() const;
    uintptr_t code_function() const;
    Primitive_Function *primitive_function() const;

private:
    std::string m_name;
    Function_Type_Info *m_fn_type;
    bool m_is_native;
    union {
        Primitive_Function *prim;
        int32_t code_ip;
    } m_fn;
};

struct Field_Info
{
    Field_Info(const std::string &name, Type_Info *type)
        : m_name(name)
        , m_type(type)
        {}

    Type_Info *type() const;
    const std::string &name() const;

private:
    std::string m_name;
    Type_Info *m_type;
};


struct Type_Info
{
    virtual ~Type_Info();
    Type_Info(Type_Info *parent, Type_Token type_token, const std::string &name)
        : m_parent(parent)
        , m_type_token(type_token)
        , m_name(name)
        {}

    Type_Info *get_parent() const;
    void set_parent(Type_Info *parent);

    Type_Token type_token() const;

    const std::string &name() const;

    bool add_field(Field_Info *field);
    const std::vector<Field_Info*> &fields() const;
    std::vector<Field_Info*> all_fields() const;

    bool add_method(Method_Info *method);
    const std::vector<Method_Info*> &methods() const;
    std::vector<Method_Info*> all_methods() const;

    bool is_subtype_of(Type_Info *other) const;
    bool is_assignable_to(Type_Info *other) const;

    Method_Info *get_method(const std::string &name, const std::vector<Type_Info*> &param_types) const;
    std::vector<Method_Info*> get_methods(const std::string &name) const;
    Field_Info *get_field(const std::string &name) const;
    bool get_field_index(const std::string &name, size_t &index) const;

private:
    void fill_methods(std::vector<Method_Info*> &v) const;
    void fill_fields(std::vector<Field_Info*> &v) const;
    bool has_method(Method_Info *method) const;
    bool has_field(const std::string &name) const;
    Method_Info *find_method(const std::string &name, const std::vector<Type_Info*> &param_types, size_t &index) const;
    Field_Info *find_field(const std::string &name, size_t &index) const;
    Type_Info *m_parent;
    Type_Token m_type_token;
    std::string m_name;
    std::vector<Field_Info*> m_fields;
    std::vector<Method_Info*> m_methods;
    std::vector<Type_Info*> m_subtypes;
};

struct Function_Type_Info : Type_Info
{
    virtual ~Function_Type_Info(){};
    Function_Type_Info(Type_Info *parent, Type_Token type_token, const std::string &name, Type_Info *return_type, const std::vector<Type_Info*> &parameter_types, bool is_native)
        : Type_Info(parent, type_token, name)
        , m_return_type(return_type)
        , m_parameter_types(std::move(parameter_types))
        , m_is_native(is_native)
        {}

    Type_Info *return_type() const;
    const std::vector<Type_Info*> &parameter_types() const;
    bool is_native() const;
private:
    Type_Info *m_return_type;
    std::vector<Type_Info *> m_parameter_types;
    bool m_is_native;
};

struct Array_Type_Info : Type_Info
{
    virtual ~Array_Type_Info(){};
    Array_Type_Info(Type_Info *parent, Type_Token type_token, const std::string &name, Type_Info *of_type)
        : Type_Info(parent, type_token, name)
        , m_of_type(of_type)
        {}
    Type_Info *of_type() const;
private:
    Type_Info *m_of_type;
};

#endif /* MALANG_VM_REFLECTION_HPP */

