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

void Method_Info::set_function(Primitive_Function *primitive)
{
    m_is_native = true;
    m_fn.prim = primitive;
}

void Method_Info::set_function(int32_t code_ip)
{
    m_is_native = false;
    m_fn.code_ip = code_ip;
}

bool Method_Info::is_native() const
{
    return m_is_native;
}

uintptr_t Method_Info::code_function() const
{
    assert(!is_native());
    return m_fn.code_ip;
}

Primitive_Function *Method_Info::primitive_function() const
{
    assert(is_native());
    return m_fn.prim;
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

Method_Info *Type_Info::find_method(const std::string &name, const std::vector<Type_Info*> &param_types, size_t &index) const
{
    assert(!name.empty());
    if (m_parent)
    {
        if (auto meth = m_parent->find_method(name, param_types, index))
            return meth;
    }
    for (auto &&m : m_methods)
    {
        if (m->name() == name)
        {
            auto m0 = m->type();
            if (m0->parameter_types().size() == param_types.size())
            {
                bool failed = true;
                for (size_t i = 0; i < m0->parameter_types().size(); ++i)
                {   // if any single parameter type doesn't match
                    if (m0->parameter_types()[i] != param_types[i])
                    {
                        failed = false;
                        break;
                    }
                }
                if (failed)
                {
                    return m;
                }
            }
        }
        ++index;
    }
    return nullptr;
}

bool Type_Info::has_method(Method_Info *method) const
{
    assert(method);
    size_t _;
    return find_method(method->name(), method->parameter_types(), _);
}

Field_Info *Type_Info::find_field(const std::string &name, size_t &index) const
{
    assert(!name.empty());
    if (m_parent)
    {
        if (auto field = m_parent->find_field(name, index))
            return field;
    }
    for (auto &&f : m_fields)
    {
        if (f->name() == name)
        {
            return f;
        }
        index++;
    }
    return nullptr;
}

bool Type_Info::has_field(const std::string &name) const
{
    size_t _;
    return find_field(name, _);
}

bool Type_Info::add_method(Method_Info *method)
{
    assert(method);
    assert(!method->name().empty());

    if (has_method(method))
    {
        return false;
    }
    m_methods.push_back(method);
    return true;
}

const std::vector<Method_Info*> &Type_Info::methods() const
{
    return m_methods;
}

void Type_Info::fill_methods(std::vector<Method_Info*> &v) const
{
    if (m_parent)
    {
        m_parent->fill_methods(v);
    }
    v.insert(v.end(), m_methods.begin(), m_methods.end());
}

void Type_Info::fill_fields(std::vector<Field_Info*> &v) const
{
    if (m_parent)
    {
        m_parent->fill_fields(v);
    }
    v.insert(v.end(), m_fields.begin(), m_fields.end());
}

std::vector<Method_Info*> Type_Info::all_methods() const
{
    std::vector<Method_Info*> all_methods;
    fill_methods(all_methods);
    return all_methods;
}

std::vector<Field_Info*> Type_Info::all_fields() const
{
    std::vector<Field_Info*> all_fields;
    fill_fields(all_fields);
    return all_fields;
}

bool Type_Info::is_assignable_to(Type_Info *other) const
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
    return false;
}

Method_Info *Type_Info::get_method(const std::string &name, const std::vector<Type_Info*> &param_types) const
{
    size_t _;
    return find_method(name, param_types, _);
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
    size_t _;
    return find_field(name, _);
}

bool Type_Info::get_field_index(const std::string &name, size_t &index) const
{
    size_t i = 0;
    if (find_field(name, i))
    {
        index = i;
        return true;
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

const std::vector<Type_Info*> &Function_Type_Info::parameter_types() const
{
    return m_parameter_types;
}
Type_Info *Function_Type_Info::return_type() const
{
    return m_return_type;
}

bool Function_Type_Info::is_native() const
{
    return m_is_native;
}

