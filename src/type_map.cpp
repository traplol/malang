#include <cassert>
#include <sstream>
#include "type_map.hpp"

Type_Map::Type_Map()
{
    m_void   = declare_type("void", nullptr);
    m_object = declare_type("object", nullptr);
    m_int    = declare_type("int", m_object);
    m_char   = declare_type("char", m_object);
    m_double = declare_type("double", m_object);
    m_string = declare_type("string", m_object);
    m_bool   = declare_type("bool", m_object);
}

Type_Info *Type_Map::get_void() const
{
    assert(m_void);
    return m_void;
}
Type_Info *Type_Map::get_object() const
{
    assert(m_object);
    return m_object;
}
Type_Info *Type_Map::get_int() const
{
    assert(m_int);
    return m_int;
}
//Type_Info *Type_Map::get_char() const
//{
//    assert(m_char);
//    return m_char;
//}
Type_Info *Type_Map::get_double() const
{
    assert(m_double);
    return m_double;
}
Type_Info *Type_Map::get_string() const
{
    assert(m_string);
    return m_string;
}
Type_Info *Type_Map::get_bool() const
{
    assert(m_bool);
    return m_bool;
}

Type_Info *Type_Map::get_or_declare_type(const std::string &name)
{
    auto existing = get_type(name);
    if (existing)
    {
        return existing;
    }
    return declare_type(name, nullptr);
}

Type_Info *Type_Map::declare_type(const std::string &name, Type_Info *parent)
{
    assert(!name.empty());
    assert(get_type(name) == nullptr);

    auto type = new Type_Info(parent, m_types_fast.size(), name);
    m_types[type->name()] = type;
    m_types_fast.push_back(type);
    assert(m_types_fast[type->type_token()] == type);
    return type;
}

static
std::string create_function_typename(Type_Info *return_type, const std::vector<Type_Info*> parameter_types, bool is_primitive)
{
    // This should generate a string in the form of:
    // "fn (int, string, MyObj) -> void"
    assert(return_type);
    std::stringstream ss;
    ss << (is_primitive ? "pfn (" : "fn (");
    for (size_t i = 0; i < parameter_types.size(); ++i)
    {
        auto pt = parameter_types[i];
        assert(pt);
        ss << pt->name();
        if (i + 1 < parameter_types.size())
        {
            ss << ", ";
        }
    }
    ss << ") -> " << return_type->name();
    return ss.str();
}

Function_Type_Info *Type_Map::declare_function(const std::vector<Type_Info*> &parameter_types, Type_Info *return_type, bool is_primitive)
{
    assert(return_type);
    auto type_name = create_function_typename(return_type, parameter_types, is_primitive);
    if (auto exists = get_type(type_name))
    {
        auto fn_ty = dynamic_cast<Function_Type_Info*>(exists);
        assert(fn_ty);
        return fn_ty;
    }
    auto fn_type = new Function_Type_Info{nullptr, m_types_fast.size(), type_name, return_type, parameter_types, is_primitive};
    m_types[fn_type->name()] = fn_type;
    m_types_fast.push_back(fn_type);
    assert(m_types_fast[fn_type->type_token()] == fn_type);
    return fn_type;
}

static
std::string create_array_type_name(Type_Info *of_type)
{
    // []int         array of (int)
    // [][]bool      array of (array of (bool))
    assert(of_type);
    std::stringstream ss;
    ss << "[]" << of_type->name();
    return ss.str();
}

Array_Type_Info *Type_Map::get_array_type(Type_Info *of_type)
{
    auto array_type_name = create_array_type_name(of_type);
    if (auto exists = get_type(array_type_name))
    {
        auto arr_ty = dynamic_cast<Array_Type_Info*>(exists);
        assert(arr_ty);
        return arr_ty;
    }
    auto arr_type = new Array_Type_Info{nullptr, m_types_fast.size(), array_type_name, of_type};
    m_types[arr_type->name()] = arr_type;
    m_types_fast.push_back(arr_type);
    assert(m_types_fast[arr_type->type_token()] == arr_type);
    return arr_type;
}

Type_Info *Type_Map::get_type(const std::string &name)
{
    assert(!name.empty());
    auto it = m_types.find(name);
    if (it != m_types.end())
    {
        return it->second;
    }
    return nullptr;
}

Type_Info *Type_Map::get_type(Type_Token type_token)
{
    assert(type_token < m_types_fast.size());
    return m_types_fast[type_token];
}
