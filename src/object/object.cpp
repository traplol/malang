#include <sstream>
#include "objects.hpp"

std::string Mal_Object::to_string() const
{
    std::stringstream ss;
    ss << "Object#" << this;
    return ss.str();
}

std::string Mal_Object::inspect() const
{
    std::stringstream ss;
    ss << "Object#" << this;
    return ss.str();
}

Type *Mal_Object::type() const
{
    return Type::get_type("Object");
}

Mal_Object *Mal_Object::call_method(Builtin_Methods id, Execution_Context &ctx, Mal_Proc_Args args)
{
    //printf("Mal_Object::call_method(%i) not impl\n", id);
    auto method = type()->methods()[id];
    return method(ctx, args);
}
Mal_Object *Mal_Object::call_method(const std::string &id, Execution_Context &ctx, Mal_Proc_Args arg)
{
    printf("Mal_Object::call_method(%s) not impl\n", id.c_str());
    return nullptr;
}

static Mal_Object *obj_to_string(Execution_Context &ctx, Mal_Proc_Args args)
{
    return new Mal_String("Object to_string not impl");
}
static Mal_Object *obj_inspect(Execution_Context &ctx, Mal_Proc_Args args)
{
    return new Mal_String("Object inspect not impl");
}

void Mal_Object::register_type()
{
    static bool registered = false;
    if (registered)
    {
        return;
    }
    auto t = Type::get_or_create_type("Object");

    t->methods()[Method_to_string] = obj_to_string;
    t->methods()[Method_inspect] = obj_inspect;

    t->define_type(nullptr);

    registered = true;
}
