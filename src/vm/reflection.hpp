#ifndef MALANG_VM_REFLECTION_HPP
#define MALANG_VM_REFLECTION_HPP

#include <string>
#include <vector>

struct Parameter_Info
{
    std::string name;
    struct Type_Info *type;
};

struct Method_Info
{
    std::string name;
    struct Type_Info *return_type;
    std::vector<Parameter_Info*> parameters;
    /*
    char is_varargs : 1;
    char is_concrete : 1;
    */
};

struct Field_Info
{
    std::string name;
    struct Type_Info *type;
};

struct Type_Info
{
    std::string name;
    std::vector<Field_Info*> fields;
    std::vector<Method_Info*> methods;
};


#endif /* MALANG_VM_REFLECTION_HPP */
