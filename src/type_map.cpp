#include <cassert>
#include <sstream>
#include "type_map.hpp"
#include "module_map.hpp"

Type_Map::~Type_Map()
{
    for (auto &&t : m_types_fast)
    {
        delete t;
    }
    m_types_fast.clear();
    m_types.clear();
}

Type_Map::Type_Map()
{
    m_module = nullptr;
    constexpr bool managed = true;
    constexpr bool not_managed = false;
    m_void   = declare_builtin_type("void",   nullptr, not_managed);
    m_buffer = declare_builtin_type("buffer", nullptr, managed);
    m_int    = declare_builtin_type("int",    nullptr, not_managed);
    m_char   = declare_builtin_type("char",   nullptr, not_managed);
    m_double = declare_builtin_type("double", nullptr, not_managed);
    m_bool   = declare_builtin_type("bool",   nullptr, not_managed);
    m_string = declare_builtin_type("string", nullptr, managed);
}

Type_Info *Type_Map::get_void() const
{
    assert(m_void);
    return m_void;
}
Type_Info *Type_Map::get_int() const
{
    assert(m_int);
    return m_int;
}
Type_Info *Type_Map::get_char() const
{
    assert(m_char);
    return m_char;
}
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
Type_Info *Type_Map::get_buffer() const
{
    assert(m_buffer);
    return m_buffer;
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
Module *Type_Map::module() const
{
    return m_module;
}
void Type_Map::module(Module *mod)
{
    m_module = mod;
}

static
std::string qualify_name(Module *mod, const std::string &name)
{
    if (!mod || name.find("::") != std::string::npos)
    {
        return name;
    }
    return mod->fully_qualified_name() + "::" + name;
}

Type_Info *Type_Map::declare_type(const std::string &name, Type_Info *parent, bool is_builtin, bool is_gc_managed)
{
    assert(!name.empty());
    assert(get_type(name) == nullptr);

    auto qualified_name = qualify_name(m_module, name);
    auto type = new Type_Info(parent, m_types_fast.size(), qualified_name);
    type->m_is_builtin = is_builtin;
    type->m_is_gc_managed = is_gc_managed;
    m_types[type->name()] = type;
    m_types_fast.push_back(type);
    assert(m_types_fast[type->type_token()] == type);
    return type;
}
Type_Info *Type_Map::declare_type(const std::string &name, Type_Info *parent)
{
    constexpr bool is_builtin = false;
    constexpr bool is_gc_managed = true;
    return declare_type(name, parent, is_builtin, is_gc_managed);
}

Type_Info *Type_Map::declare_builtin_type(const std::string &name, Type_Info *parent, bool gc_managed)
{
    constexpr bool is_builtin = true;
    return declare_type(name, parent, is_builtin, gc_managed);
}

static
std::string create_function_typename(Type_Info *return_type, const std::vector<Type_Info*> parameter_types, bool is_native)
{
    // This should generate a string in the form of:
    // "fn (int, string, MyType) -> void"
    assert(return_type);
    std::stringstream ss;
    ss << (is_native ? "pfn (" : "fn (");
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

Function_Type_Info *Type_Map::declare_function(const Types &parameter_types, Type_Info *return_type, bool is_native)
{
    assert(return_type);
    auto type_name = create_function_typename(return_type, parameter_types, is_native);
    if (auto exists = get_type(type_name))
    {
        auto fn_ty = dynamic_cast<Function_Type_Info*>(exists);
        assert(fn_ty);
        return fn_ty;
    }
    auto type_token = static_cast<Type_Token>(m_types_fast.size());
    auto fn_type = new Function_Type_Info{nullptr, type_token, type_name, return_type, parameter_types, is_native};
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
    auto type_token = static_cast<Type_Token>(m_types_fast.size());
    auto arr_type = new Array_Type_Info{type_token, array_type_name, of_type};
    auto length_field = new Field_Info{"length", m_int, true, false};
    arr_type->add_field(length_field);
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
    auto qualified_name = qualify_name(m_module, name);
    it = m_types.find(qualified_name);
    if (it != m_types.end())
    {
        return it->second;
    }
    return nullptr;
}

Type_Info *Type_Map::get_type(Type_Token type_token)
{
    auto size = m_types_fast.size();
    auto idx = static_cast<decltype(size)>(type_token);
    assert(idx < size);
    return m_types_fast[idx];
}

void Type_Map::dump() const
{
    printf(">>>>>>>>>>>>TYPE DUMP<<<<<<<<<<\n");
    for (auto t : m_types_fast)
    {
        t->dump();
    }
}
