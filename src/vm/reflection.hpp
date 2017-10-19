#ifndef MALANG_VM_REFLECTION_HPP
#define MALANG_VM_REFLECTION_HPP

#include <string>
#include <vector>

struct Function_Type_Info;
struct Type_Info;

using Primitive_Function = void(*)(struct Malang_VM &vm);

struct Method_Info
{
    Method_Info(const std::string &name, Function_Type_Info *fn_type)
        : m_name(name)
        , m_fn_type(fn_type)
        , m_is_primitive(false)
        {}

    Method_Info(const std::string &name, Function_Type_Info *fn_type, Primitive_Function prim)
        : m_name(name)
        , m_fn_type(fn_type)
        {
            set_function(prim);
        }

    Method_Info(const std::string &name, Function_Type_Info *fn_type, uintptr_t code_ip)
        : m_name(name)
        , m_fn_type(fn_type)
        {
            set_function(code_ip);
        }

    Function_Type_Info *type() const;
    const std::string &name() const;
    const std::vector<Type_Info*> &parameter_types() const;
    Type_Info *return_type() const;

    void set_function(Primitive_Function primitive);
    void set_function(uintptr_t code_ip);
    bool is_primitive() const;
    uintptr_t code_function() const;
    Primitive_Function primitive_function() const;

private:
    std::string m_name;
    Function_Type_Info *m_fn_type;
    bool m_is_primitive;
    union {
        Primitive_Function primitive;
        uintptr_t code_ip;
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


using Type_Token = size_t;

struct Type_Info
{
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

    bool add_method(Method_Info *method);
    const std::vector<Method_Info*> &methods() const;

    bool castable_to(Type_Info *other) const;
    bool is_subtype_of(Type_Info *other) const;

    Method_Info *get_method(const std::string &name, std::vector<Type_Info*> param_types) const;
    std::vector<Method_Info*> get_methods(const std::string &name) const;
    Field_Info *get_field(const std::string &name) const;

private:
    Type_Info *m_parent;
    Type_Token m_type_token;
    std::string m_name;
    std::vector<Field_Info*> m_fields;
    std::vector<Method_Info*> m_methods;
    std::vector<Type_Info*> m_subtypes;
};

struct Function_Type_Info : Type_Info
{
    Function_Type_Info(Type_Info *parent, Type_Token type_token, const std::string &name, Type_Info *return_type, const std::vector<Type_Info*> parameter_types)
        : Type_Info(parent, type_token, name)
        , m_return_type(return_type)
        , m_parameter_types(std::move(parameter_types))
        {}

    Type_Info *return_type() const;
    const std::vector<Type_Info*> &parameter_types() const;

private:
    Type_Info *m_return_type;
    std::vector<Type_Info *> m_parameter_types;
};


#endif /* MALANG_VM_REFLECTION_HPP */
