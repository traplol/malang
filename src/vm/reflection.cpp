#include <cassert>
#include "reflection.hpp"

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
        if (f == field || f->name == field->name)
        {
            return false;
        }
    }
    m_fields.push_back(field);
    return true;
}

const std::vector<Field_Info*> Type_Info::fields() const
{
    return m_fields;
}

bool Type_Info::add_method(Method_Info *method)
{
    assert(method);

    for (auto &&m : m_methods)
    {
        if (m == method || m->name == method->name)
        {
            return false;
        }
    }
    m_methods.push_back(method);
    return true;
}

const std::vector<Method_Info*> Type_Info::methods() const
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
        if (m->return_type == this && m->name == "cast_to")
        {
            return true;
        }
    }

    return false;
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
