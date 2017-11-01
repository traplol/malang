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
    
    Bound_Function(Native_Function *native)
        : m_is_native(true)
        , m_is_valid(true)
        , m_fn_type(native->fn_type)
    {
        m_u.native = native;
    }
    Bound_Function(Function_Type_Info *fn_type, IR_Label *code)
        : m_is_native(false)
        , m_is_valid(true)
        , m_fn_type(fn_type)
    {
        m_u.code = code;
    }

    Bound_Function() : m_is_valid(false) {}

    inline bool is_native() const { return m_is_native; }
    inline bool is_valid() const { return m_is_valid; }
    inline IR_Label *code() const
    {
        assert(is_valid());
        assert(!is_native());
        return m_u.code;
    }
    inline Native_Function *native() const
    {
        assert(is_valid());
        assert(is_native());
        return m_u.native;
    }
    inline Function_Type_Info *fn_type() const
    {
        assert(is_valid());
        return m_fn_type;
    }
private:
    bool m_is_native;
    bool m_is_valid;
    union {
        IR_Label *code;
        Native_Function *native;
    } m_u;
    Function_Type_Info *m_fn_type;
};

using Bound_Functions = std::vector<Bound_Function>;

struct Bound_Function_Map
{
    ~Bound_Function_Map();
    bool add_method(Type_Info *to_type, const std::string &name, Function_Type_Info *fn_type, Native_Code native);
    bool add_method(Type_Info *to_type, const std::string &name, Function_Type_Info *fn_type, IR_Label *code);
    bool add(const std::string &name, Function_Type_Info *fn_type, Native_Code native);
    bool add(const std::string &name, Function_Type_Info *fn_type, IR_Label *code);
    Bound_Function get(const std::string &name, const Function_Parameters &params) const;
    Bound_Functions get(const std::string &name) const;

    bool any(const std::string &name) const;
    void dump() const;

    std::vector<Native_Code> natives() const;
private:
    using Params_To_Function_Map = std::unordered_map<Function_Parameters, Bound_Function>;
    using Name_To_Params_To_Bound_Function_Map = std::map<std::string, Params_To_Function_Map>;
    Name_To_Params_To_Bound_Function_Map m_free_functions;
    std::vector<Native_Code> m_all_natives;
    std::vector<Native_Function*> m_natives_to_free;
};

#endif /* MALANG_IR_BOUND_FUNCTION_MAP_HPP */
