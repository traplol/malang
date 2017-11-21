#include <cassert>
#include <sstream>
#include "reflection.hpp"

std::string Function_Parameters::to_string() const
{
    std::stringstream ss;
    ss << "(";
    for (size_t i = 0; i < m_parameter_types.size(); ++i)
    {
        ss << m_parameter_types[i]->name();
        if (i + 1 < m_parameter_types.size())
        {
            ss << ", ";
        }
    }
    ss << ")";
    return ss.str();
}

bool Trait::matches(const std::string &name, const Function_Type_Info *fn_type) const
{
    assert(!name.empty());
    if (!fn_type) return false;
    if (m_name != name) return false;
    return m_fn_type == fn_type;
}

bool Trait_Definition::is_implemented_by(const Type_Info *type) const
{
    for (auto &&t : m_traits)
    {
        if (auto meth = type->get_method(t->name(), t->fn_type()->parameter_types()))
        {
            if (!t->matches(meth->name(), meth->type()))
            {
                return false;
            }
        }
    }
    return true;
}

Constructor_Info::~Constructor_Info()
{
    if (m_is_native)
    {
        delete m_fn.prim;
        m_fn.prim = nullptr;
    }
}

Function_Type_Info *Constructor_Info::type() const
{
    return m_fn_type;
}

const Function_Parameters &Constructor_Info::parameter_types() const
{
    assert(m_fn_type);
    return m_fn_type->parameter_types();
}

void Constructor_Info::set_function(Native_Function *native)
{
    m_is_native = true;
    m_fn.prim = native;
}

void Constructor_Info::set_function(IR_Label *code_ip)
{
    m_is_native = false;
    m_fn.code_ip = code_ip;
}

bool Constructor_Info::is_native() const
{
    return m_is_native;
}

bool Constructor_Info::is_the_default_ctor() const
{   // The default constructor is one that has only been declared to exist and
    // takes no arguments. It ultimately will not be called as it has no body,
    // and this allows the compiler to only bother with a call to .init
    return m_fn_type->parameter_types().empty()
        && is_waiting_for_definition();
}

bool Constructor_Info::is_waiting_for_definition() const
{
    return m_fn.code_ip == nullptr;
}

IR_Label *Constructor_Info::code_function() const
{
    assert(!is_native());
    return m_fn.code_ip;
}

Native_Function *Constructor_Info::native_function() const
{
    assert(is_native());
    return m_fn.prim;
}

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

bool Method_Info::is_waiting_for_definition() const
{
    return m_fn.code_ip == nullptr;
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

void Type_Info::aliased_to(Type_Info *type)
{
    assert(type);
    if (type == this)
    {
        printf("cannot alias %s itself\n", m_name.c_str());
        abort();
    }
    if (m_aliased_to)
    {
        printf("cannot alias %s to %s because it is already aliased to %s\n",
               m_name.c_str(), m_aliased_to->m_name.c_str(), type->m_name.c_str());
        abort();
    }

    m_aliased_to = type;
    auto t = m_aliased_to;
    while (t)
    {   // Check for cycles
        if (t->m_cycle)
        {
            printf("Cyclic type alias detected for %s and %s\n",
                   name().c_str(),
                   type->name().c_str());
            abort();
        }
        t->m_cycle = true;
        t = t->m_aliased_to;
    }

    t = m_aliased_to;
    while (t)
    {   // undo check
        t->m_cycle = false;
        t = t->m_aliased_to;
    }

    m_type_token = type->m_type_token;
}
Type_Info *Type_Info::aliased_to()
{
    return m_aliased_to ? m_aliased_to : this;
}

const Type_Info *Type_Info::const_aliased_to_top() const
{
    auto cur = this;
    while (cur->m_aliased_to)
    {
        cur = cur->m_aliased_to;
    }
    return cur;
}

Type_Info *Type_Info::aliased_to_top()
{
    // ehh
    return const_cast<Type_Info*>(const_aliased_to_top());
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
    auto top = const_aliased_to_top();
    return top->m_fields;
}

Constructor_Info *Type_Info::init()
{
    auto top = aliased_to_top();
    return top->m_init;
}
void Type_Info::init(Constructor_Info *init)
{
    auto top = aliased_to_top();
    assert(top->m_init == nullptr);
    top->m_init = init;
}

const Constructors &Type_Info::constructors() const
{
    return m_constructors;
}

Constructor_Info *Type_Info::get_constructor(const Function_Parameters &param_types) const
{
    auto top = const_aliased_to_top();

    for (auto &&c : top->m_constructors)
    {
        if (c->parameter_types() == param_types)
            return c;
    }
    return nullptr;
}

bool Type_Info::add_constructor(Constructor_Info *ctor)
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

    auto cur = this;
    while (cur)
    {
        for (auto &&m : cur->m_methods)
        {
            if (m->name() == name)
            {
                if (m->parameter_types() == param_types)
                    return m;
            }
            ++index;
        }
        cur = cur->m_aliased_to;
    }
    return nullptr;
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
    auto top = const_aliased_to_top();
    for (auto &&f : top->m_fields)
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
    if (m_aliased_to)
    {
        m_aliased_to->fill_methods(v);
    }
    v.insert(v.end(), m_methods.begin(), m_methods.end());
}

void Type_Info::fill_fields(Fields &v) const
{
    auto top = const_aliased_to_top();
    v.insert(v.end(), top->m_fields.begin(), top->m_fields.end());
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

bool Type_Info::has_no_init() const
{
    return is_builtin();
}
bool Type_Info::is_builtin() const
{
    return const_aliased_to_top()->m_is_builtin;
}
bool Type_Info::is_gc_managed() const
{
    return m_is_gc_managed;
}
bool Type_Info::is_assignable_to(const Type_Info *other) const
{
    if (this == other)
    {
        return true;
    }
    return other->is_alias_to(this);
}

bool Type_Info::implemented_trait(const Trait_Definition *trait) const
{
    return trait->is_implemented_by(this);
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

bool Type_Info::is_alias_to(const Type_Info *other) const
{
    assert(other);
    if (this == other)
    {
        return true;
    }
    auto p = m_aliased_to;
    while (p)
    {
        if (p == other)
        {
            return true;
        }
        p = p->m_aliased_to;
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

void Type_Info::dump() const
{
    printf("%s\n", name().c_str());
    printf("  Fields:\n");
    for (auto &&f : all_fields())
    {
        printf("    %s: %s", f->name().c_str(), f->type()->name().c_str());
        if (f->is_readonly())
        {
            printf(" (readonly)");
        }
        printf("\n");
    }
    printf("  Ctors:\n");
    for (auto &&c : constructors())
    {
        printf("    new %s", c->parameter_types().to_string().c_str());
        if (c->is_native())
        {
            printf(" (native)");
        }
        printf("\n");
    }
    printf("  Methods:\n");
    for (auto &&m : all_methods())
    {
        printf("    fn %s %s -> %s",
               m->name().c_str(),
               m->parameter_types().to_string().c_str(),
               m->return_type()->name().c_str());
        if (m->is_native())
        {
            printf(" (native)");
        }
        printf("\n");
    }
}
