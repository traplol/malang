#ifndef MALANG_VM_RUNTIME_PRIMITIVE_FUNCTION_MAP
#define MALANG_VM_RUNTIME_PRIMITIVE_FUNCTION_MAP

#include <vector>
#include <map>
#include <string>
#include "primitive_types.hpp"


struct Primitive_Function_Map
{
    ~Primitive_Function_Map();
    std::vector<Native_Code> primitives() const;
    Primitive_Function *add_primitive(struct Function_Type_Info *fn_type, Native_Code native_code);
    Primitive_Function *add_builtin(const std::string &name, struct Function_Type_Info *fn_type, Native_Code native_code);
    Primitive_Function *get_builtin(const std::string &name);
private:
    std::vector<Native_Code> m_primitives;
    std::map<std::string, Primitive_Function*> m_builtins;
};

#endif /* MALANG_VM_RUNTIME_PRIMITIVE_FUNCTION_MAP */
