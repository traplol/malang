#ifndef MALANG_OBJECT_OBJECT_HPP
#define MALANG_OBJECT_OBJECT_HPP

#include <vector>
#include "../type_info.hpp"

#define OBJECT_OVERRIDES \
    std::string to_string() const override; \
    std::string inspect() const override; \
    Type *type() const override

struct Mal_Object
{
    virtual ~Mal_Object(){}
    virtual std::string to_string() const;
    virtual std::string inspect() const;
    virtual Type *type() const;
    Mal_Object *call_method(Builtin_Methods id, Execution_Context &ctx, Mal_Proc_Args args);
    Mal_Object *call_method(const std::string &id, Execution_Context &ctx, Mal_Proc_Args args);

    static void register_type();
};

#endif /* MALANG_OBJECT_OBJECT_HPP */
