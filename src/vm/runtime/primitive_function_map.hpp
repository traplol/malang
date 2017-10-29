#ifndef MALANG_VM_RUNTIME_PRIMITIVE_FUNCTION_MAP
#define MALANG_VM_RUNTIME_PRIMITIVE_FUNCTION_MAP

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include "primitive_types.hpp"
#include "reflection.hpp"


struct Primitive_Function_Map
{
    ~Primitive_Function_Map();
    std::vector<Native_Code> primitives() const;
    Primitive_Function *add_primitive(const std::string &name, Function_Type_Info *fn_type, Native_Code native_code);
    Primitive_Function *add_builtin(const std::string &name, Function_Type_Info *fn_type, Native_Code native_code);
    Primitive_Function *get_builtin(const std::string &name, const Function_Parameters &params);
    bool builtin_exists(const std::string &name);
    std::vector<Primitive_Function*> get_builtins(const std::string &name);
    void dump() const;
private:
    using Params_To_Function_Map = std::unordered_map<Function_Parameters, Primitive_Function*>;
    using Name_To_Params_To_Function_Map = std::map<std::string, Params_To_Function_Map>;
    Name_To_Params_To_Function_Map m_builtins;
    std::vector<Native_Code> m_primitives;
};

#endif /* MALANG_VM_RUNTIME_PRIMITIVE_FUNCTION_MAP */
