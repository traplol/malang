#ifndef MALANG_VM_REFLECTION_HPP
#define MALANG_VM_REFLECTION_HPP

#include <string>
#include <vector>

struct Parameter_Info
{
    struct Type_Info *type;
    std::string name;
};

struct Method_Info
{
    struct Type_Info *return_type;
    std::string name;
    std::vector<Parameter_Info*> parameters;
};

struct Field_Info
{
    struct Type_Info *type;
    std::string name;
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
    const std::vector<Field_Info*> fields() const;

    bool add_method(Method_Info *method);
    const std::vector<Method_Info*> methods() const;

    bool castable_to(Type_Info *other) const;
    bool is_subtype_of(Type_Info *other) const;

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
    const std::vector<Type_Info*> parameter_types() const;

private:
    Type_Info *m_return_type;
    std::vector<Type_Info *> m_parameter_types;
};


#endif /* MALANG_VM_REFLECTION_HPP */
