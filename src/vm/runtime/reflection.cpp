#include <cassert>
#include "reflection.hpp"

Method_Info::~Method_Info()
{
    if (m_is_native)
    {
        delete m_fn.prim;
        m_fn.prim = nullptr;
    }
}

Function_Type_Info *Method_Info::type() const
{
    return m_fn_type;
}

const Function_Parameters &Method_Info::parameter_types() const
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

void Method_Info::set_function(Native_Function *native)
{
    m_is_native = true;
    m_fn.prim = native;
}

void Method_Info::set_function(IR_Label *code_ip)
{
    m_is_native = false;
    m_fn.code_ip = code_ip;
}

bool Method_Info::is_native() const
{
    return m_is_native;
}

IR_Label *Method_Info::code_function() const
{
    assert(!is_native());
    return m_fn.code_ip;
}

Native_Function *Method_Info::native_function() const
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

bool Field_Info::is_readonly() const
{
    return m_is_readonly;
}

Num_Fields_Limit Field_Info::index() const
{
    return m_index;
}

Type_Info::~Type_Info()
{
    for (auto &&f : m_fields)
    {
        delete f;
    }
    m_fields.clear();
    for (auto &&m : m_methods)
    {
        delete m;
    }
    m_methods.clear();
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
    // @FixMe: Look at parent types, still unsure about supporting polymorphism?
    assert(field);

    for (auto &&f : m_fields)
    {
        if (f == field || f->name() == field->name())
        {
            return false;
        }
    }
    if (static_cast<Num_Fields_Limit>(m_fields.size()) != m_fields.size())
    {
        printf("Too many fields in type `%s'", name().c_str());
        abort();
    }
    field->m_index = m_fields.size();
    m_fields.push_back(field);
    return true;
}

const Fields &Type_Info::fields() const
{
    return m_fields;
}

static
Method_Info *find_method_impl(const Methods &methods, const std::string &name, const Function_Parameters &param_types, Num_Fields_Limit &index)
{
    for (auto &&m : methods)
    {
        if (m->name() == name)
        {
            if (m->parameter_types() == param_types)
                return m;
        }
        ++index;
    }
    return nullptr;
}

Method_Info *Type_Info::get_constructor(const Function_Parameters &param_types) const
{
    Num_Fields_Limit _ = 0;
    return find_method_impl(m_constructors, ".ctor", param_types, _);
}

bool Type_Info::add_constructor(Method_Info *ctor)
{
    assert(ctor);
    if (get_constructor(ctor->parameter_types()))
    {
        return false;
    }
    m_constructors.push_back(ctor);
    return true;
}

Method_Info *Type_Info::find_method(const std::string &name, const Function_Parameters &param_types, Num_Fields_Limit &index) const
{
    assert(!name.empty());
    if (m_parent)
    {
        if (auto meth = m_parent->find_method(name, param_types, index))
            return meth;
    }
    return find_method_impl(m_methods, name, param_types, index);
}

bool Type_Info::has_method(Method_Info *method) const
{
    assert(method);
    Num_Fields_Limit _;
    return find_method(method->name(), method->parameter_types(), _);
}

Field_Info *Type_Info::find_field(const std::string &name, Num_Fields_Limit &index) const
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
    Num_Fields_Limit _;
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

const Methods &Type_Info::methods() const
{
    return m_methods;
}

void Type_Info::fill_methods(Methods &v) const
{
    if (m_parent)
    {
        m_parent->fill_methods(v);
    }
    v.insert(v.end(), m_methods.begin(), m_methods.end());
}

void Type_Info::fill_fields(Fields &v) const
{
    if (m_parent)
    {
        m_parent->fill_fields(v);
    }
    v.insert(v.end(), m_fields.begin(), m_fields.end());
}

Methods Type_Info::all_methods() const
{
    Methods all_methods;
    fill_methods(all_methods);
    return all_methods;
}

Fields Type_Info::all_fields() const
{
    Fields all_fields;
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

Method_Info *Type_Info::get_method(const std::string &name, const Function_Parameters &param_types) const
{
    Num_Fields_Limit _;
    return find_method(name, param_types, _);
}

Methods Type_Info::get_methods(const std::string &name) const
{
    Methods methods;
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
    Num_Fields_Limit _;
    return find_field(name, _);
}

bool Type_Info::get_field_index(const std::string &name, Num_Fields_Limit &index) const
{
    Num_Fields_Limit i = 0;
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

const Function_Parameters &Function_Type_Info::parameter_types() const
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

Type_Info *Array_Type_Info::of_type() const
{
    return m_of_type;
}
