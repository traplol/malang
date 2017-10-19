#include <cassert>
#include "reflection.hpp"

Function_Type_Info *Method_Info::type() const
{
    return m_fn_type;
}

const std::vector<Type_Info*> &Method_Info::parameter_types() const
{
    assert(m_fn_type);
    return m_fn_type->parameter_types();
}

Type_Info *Method_Info::return_type() const
{
    assert(m_fn_type);
    return m_fn_type->return_type();
}

const std::string &Method_Info::name() const
{
    return m_name;
}

void Method_Info::set_function(Primitive_Function primitive)
{
    m_is_primitive = true;
    m_fn.primitive = primitive;
}

void Method_Info::set_function(uintptr_t code_ip)
{
    m_is_primitive = false;
    m_fn.code_ip = code_ip;
}

bool Method_Info::is_primitive() const
{
    return m_is_primitive;
}

uintptr_t Method_Info::code_function() const
{
    assert(!is_primitive());
    return m_fn.code_ip;
}

Primitive_Function Method_Info::primitive_function() const
{
    assert(is_primitive());
    return m_fn.primitive;
}

Type_Info *Field_Info::type() const
{
    return m_type;
}

const std::string &Field_Info::name() const
{
    return m_name;
}

Type_Info *Type_Info::get_parent() const
{
    return m_parent;
}

void Type_Info::set_parent(Type_Info *parent)
{
    assert(m_parent == nullptr);
    m_parent = parent;
    m_parent->m_subtypes.push_back(this);
}

Type_Token Type_Info::type_token() const
{
    return m_type_token;
}

const std::string &Type_Info::name() const
{
    return m_name;
}

bool Type_Info::add_field(Field_Info *field)
{
    assert(field);

    for (auto &&f : m_fields)
    {
        if (f == field || f->name() == field->name())
        {
            return false;
        }
    }
    m_fields.push_back(field);
    return true;
}

const std::vector<Field_Info*> &Type_Info::fields() const
{
    return m_fields;
}

bool Type_Info::add_method(Method_Info *method)
{
    assert(method);

    for (auto &&m : m_methods)
    {
        if (m == method)
        {
            return false;
        }
        if (m->name() == method->name())
        {
            auto m0 = m->type();
            auto m1 = method->type();
            if (m0 == m1)
            {
                return false;
            }
            // return type could be different at this point so we still must check param types
            if (m0->parameter_types().size() == m1->parameter_types().size())
            {
                bool failed = true;
                for (size_t i = 0; i < m0->parameter_types().size(); ++i)
                {
                    if (m0->parameter_types()[i] != m1->parameter_types()[i])
                    {
                        failed = false;
                        break;
                    }
                }
                if (failed)
                {
                    return false;
                }
            }
        }
    }
    m_methods.push_back(method);
    return true;
}

const std::vector<Method_Info*> &Type_Info::methods() const
{
    return m_methods;
}

bool Type_Info::castable_to(Type_Info *other) const
{
    if (this == other)
    {
        return true;
    }
    assert(m_type_token != other->m_type_token);

    if (is_subtype_of(other))
    {
        return true;
    }

    for (auto &&m : other->m_methods)
    {
        // @XXX: magic string: "cast_to"
        if (m->type()->return_type() == this && m->name() == "cast_to")
        {
            return true;
        }
    }

    return false;
}

Method_Info *Type_Info::get_method(const std::string &name, std::vector<Type_Info*> param_types) const
{
    // @TODO: get_method should look up the type tree
    for (auto &&m : m_methods)
    {
        if (m->name() == name
            && param_types.size() == m->type()->parameter_types().size())
        {
            bool success = true;
            for (size_t i = 0; i < param_types.size(); ++i)
            {
                if (param_types[i] != m->type()->parameter_types()[i])
                {
                    success = false;
                    break;
                }
            }
            if (success)
            {
                return m;
            }
        }
    }
    return nullptr;
}

std::vector<Method_Info*> Type_Info::get_methods(const std::string &name) const
{
    // @TODO: get_methods should look up the type tree
    std::vector<Method_Info*> methods;
    for (auto &&m : m_methods)
    {
        if (m->name() == name)
        {
            methods.push_back(m);
        }
    }
    return methods;
}

Field_Info *Type_Info::get_field(const std::string &name) const
{
    // @TODO: get_field should look up the type tree
    for (auto &&f : m_fields)
    {
        if (f->name() == name)
        {
            return f;
        }
    }
    return nullptr;
}

bool Type_Info::is_subtype_of(Type_Info *other) const
{
    if (this == other)
    {
        return false;
    }
    auto p = m_parent;
    while (p)
    {
        if (p == other)
        {
            return true;
        }
        p = p->m_parent;
    }
    return false;
}

const std::vector<Type_Info*> &Function_Type_Info::parameter_types() const
{
    return m_parameter_types;
}
Type_Info *Function_Type_Info::return_type() const
{
    return m_return_type;
}
