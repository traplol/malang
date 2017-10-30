#ifndef MALANG_VM_REFLECTION_HPP
#define MALANG_VM_REFLECTION_HPP

#include <string>
#include <vector>
#include "primitive_types.hpp"

struct Function_Type_Info;
struct Array_Type_Info;
struct Type_Info;
struct IR_Label;
using Type_Token = Fixnum;
using Methods = std::vector<struct Method_Info*>;
using Fields = std::vector<struct Field_Info*>;
using Types = std::vector<struct Type_Info*>;

struct Function_Parameters
{
    Function_Parameters() {}
    Function_Parameters(const std::initializer_list<Type_Info*> &list)
        : m_parameter_types(list)
        {}
    Function_Parameters(const Types &copy)
        : m_parameter_types(copy)
        {}
    Function_Parameters(Types &&move)
        : m_parameter_types(std::move(move))
        {}

    const Types &types() const
    {
        return m_parameter_types;
    }

    bool operator==(const Function_Parameters &other) const
    {
        if (this == &other) return true;
        if (m_parameter_types.size() != other.m_parameter_types.size())
            return false;
        for (size_t i = 0; i < m_parameter_types.size(); ++i)
        {
            if (other[i] != (*this)[i])
                return false;
        }
        return true;
    }

    bool operator!=(const Function_Parameters &other) const
    {
        return !(*this == other);
    }

    Type_Info *operator[](size_t index) const
    {
        return m_parameter_types[index];
    }

    auto begin()  const { return m_parameter_types.begin(); }
    auto end()    const { return m_parameter_types.end(); }
    auto rbegin() const { return m_parameter_types.rbegin(); }
    auto rend()   const { return m_parameter_types.rend(); }
    auto size()   const { return m_parameter_types.size(); }

private:
    Types m_parameter_types;
};

namespace std
{
    template<>
    struct hash<Function_Parameters>
    {
        size_t operator()(const Function_Parameters &fp) const
        {
            size_t res = 17;
            for (auto &&t : fp)
            {
                res = res * 31 + hash<uintptr_t>()(reinterpret_cast<uintptr_t>(t));
            }
            return res;
        }
    };
}

struct Method_Info
{
    ~Method_Info();
    /*
    Method_Info(const std::string &name, Function_Type_Info *fn_type)
        : m_name(name)
        , m_fn_type(fn_type)
        , m_is_native(false)
        {}
    */

    Method_Info(const std::string &name, Function_Type_Info *fn_type, Primitive_Function *prim)
        : m_name(name)
        , m_fn_type(fn_type)
        {
            set_function(prim);
        }

    Method_Info(const std::string &name, Function_Type_Info *fn_type, IR_Label *code_ip)
        : m_name(name)
        , m_fn_type(fn_type)
        {
            set_function(code_ip);
        }

    Function_Type_Info *type() const;
    const std::string &name() const;
    const Function_Parameters &parameter_types() const;
    Type_Info *return_type() const;

    void set_function(Primitive_Function *prim);
    void set_function(IR_Label *code_ip);
    bool is_native() const;
    IR_Label *code_function() const;
    Primitive_Function *primitive_function() const;

private:
    std::string m_name;
    Function_Type_Info *m_fn_type;
    bool m_is_native;
    union {
        Primitive_Function *prim;
        IR_Label *code_ip;
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
        , m_is_gc_managed(true)
        , m_type_token(type_token)
        , m_name(name)
        {}

    Type_Info *get_parent() const;
    void set_parent(Type_Info *parent);

    Type_Token type_token() const;

    const std::string &name() const;

    bool add_field(Field_Info *field);
    const Fields &fields() const;
    Fields all_fields() const;

    bool add_constructor(Method_Info *ctor);
    const Methods &constructors() const;

    bool add_method(Method_Info *method);
    const Methods &methods() const;
    Methods all_methods() const;

    bool is_gc_managed() const { return m_is_gc_managed; }
    bool is_subtype_of(Type_Info *other) const;
    bool is_assignable_to(Type_Info *other) const;

    Method_Info *get_constructor(const Function_Parameters &param_types) const;
    Method_Info *get_method(const std::string &name, const Function_Parameters &param_types) const;
    Methods get_methods(const std::string &name) const;
    Field_Info *get_field(const std::string &name) const;
    bool get_field_index(const std::string &name, size_t &index) const;

private:
    friend struct Type_Map;
    void fill_methods(Methods &v) const;
    void fill_fields(Fields &v) const;
    bool has_method(Method_Info *method) const;
    bool has_field(const std::string &name) const;
    Method_Info *find_method(const std::string &name, const Function_Parameters &param_types, size_t &index) const;
    Field_Info *find_field(const std::string &name, size_t &index) const;
    Type_Info *m_parent;
    bool m_is_gc_managed;
    Type_Token m_type_token;
    std::string m_name;
    Fields m_fields;
    Methods m_methods;
    Methods m_constructors;
    Types m_subtypes;
};

struct Function_Type_Info : Type_Info
{
    virtual ~Function_Type_Info(){};
    Function_Type_Info(Type_Info *parent, Type_Token type_token, const std::string &name, Type_Info *return_type, const Function_Parameters &parameter_types, bool is_native)
        : Type_Info(parent, type_token, name)
        , m_return_type(return_type)
        , m_parameter_types(std::move(parameter_types))
        , m_is_native(is_native)
        {}

    Type_Info *return_type() const;
    const Function_Parameters &parameter_types() const;
    bool is_native() const;
private:
    Type_Info *m_return_type;
    Function_Parameters m_parameter_types;
    bool m_is_native;
};

struct Array_Type_Info : Type_Info
{
    virtual ~Array_Type_Info(){};
    Array_Type_Info(Type_Token type_token, const std::string &name, Type_Info *of_type)
        : Type_Info(nullptr, type_token, name)
        , m_of_type(of_type)
        {}
    Type_Info *of_type() const;
private:
    Type_Info *m_of_type;
};

#endif /* MALANG_VM_REFLECTION_HPP */

