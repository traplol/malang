#ifndef MALANG_IR_BOUND_FUNCTION_MAP_HPP
#define MALANG_IR_BOUND_FUNCTION_MAP_HPP

#include <map>
#include <string>
#include <unordered_map>
#include "../vm/runtime/reflection.hpp"
#include "ir_label.hpp"

struct Bound_Function
{
    // Does this need the name it is bound to?
    
    Bound_Function(Primitive_Function *primitive)
        : m_is_primitive(true)
        , m_is_valid(true)
        , m_fn_type(primitive->fn_type)
    {
        m_u.primitive = primitive;
    }
    Bound_Function(Function_Type_Info *fn_type, IR_Label *code)
        : m_is_primitive(false)
        , m_is_valid(true)
        , m_fn_type(fn_type)
    {
        m_u.code = code;
    }

    Bound_Function() : m_is_valid(false) {}

    inline bool is_primitive() const { return m_is_primitive; }
    inline bool is_valid() const { return m_is_valid; }
    inline IR_Label *code() const
    {
        assert(is_valid());
        assert(!is_primitive());
        return m_u.code;
    }
    inline Primitive_Function *primitive() const
    {
        assert(is_valid());
        assert(is_primitive());
        return m_u.primitive;
    }
    inline Function_Type_Info *fn_type() const
    {
        //assert(is_valid());
        return m_fn_type;
    }
private:
    bool m_is_primitive;
    bool m_is_valid;
    union {
        IR_Label *code;
        Primitive_Function *primitive;
    } m_u;
    Function_Type_Info *m_fn_type;
};

using Bound_Functions = std::vector<Bound_Function>;

struct Bound_Function_Map
{
    bool add_function(Primitive_Function *primitive);
    bool add_function(const std::string &name, Function_Type_Info *fn_type, IR_Label *code);
    Bound_Function get_function(const std::string &name, const Function_Parameters &params);
    Bound_Functions get_functions(const std::string &name);
    bool any(const std::string &name);
private:
    using Params_To_Function_Map = std::unordered_map<Function_Parameters, Bound_Function>;
    using Name_To_Params_To_Bound_Function_Map = std::map<std::string, Params_To_Function_Map>;
    Name_To_Params_To_Bound_Function_Map m_bound_functions;
};

#endif /* MALANG_IR_BOUND_FUNCTION_MAP_HPP */
