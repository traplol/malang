#include <sstream>
#include "objects.hpp"

std::string Mal_Integer::to_string() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
std::string Mal_Integer::inspect() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
Type *Mal_Integer::type() const
{
    return Type::get_type("Integer");
}

static Mal_Object *int_to_string(Execution_Context &ctx, Mal_Proc_Args args)
{
    auto thisptr = static_cast<Mal_Integer*>(args[0]);
    return new Mal_String(thisptr->to_string());
}
static Mal_Object *int_inspect(Execution_Context &ctx, Mal_Proc_Args args)
{
    auto thisptr = static_cast<Mal_Integer*>(args[0]);
    return new Mal_String(thisptr->inspect());
}
static Mal_Object *int_b_op_add(Execution_Context &ctx, Mal_Proc_Args args)
{
    auto thisptr = static_cast<Mal_Integer*>(args[0]);
    auto other = static_cast<Mal_Integer*>(args[1]);
    return new Mal_Integer(thisptr->value + other->value);
}
static Mal_Object *int_b_op_sub(Execution_Context &ctx, Mal_Proc_Args args)
{
    auto thisptr = static_cast<Mal_Integer*>(args[0]);
    auto other = static_cast<Mal_Integer*>(args[1]);
    return new Mal_Integer(thisptr->value - other->value);
}
static Mal_Object *int_b_op_mul(Execution_Context &ctx, Mal_Proc_Args args)
{
    auto thisptr = static_cast<Mal_Integer*>(args[0]);
    auto other = static_cast<Mal_Integer*>(args[1]);
    return new Mal_Integer(thisptr->value * other->value);
}
static Mal_Object *int_b_op_div(Execution_Context &ctx, Mal_Proc_Args args)
{
    auto thisptr = static_cast<Mal_Integer*>(args[0]);
    auto other = static_cast<Mal_Integer*>(args[1]);
    return new Mal_Integer(thisptr->value / other->value);
}
static Mal_Object *int_b_op_mod(Execution_Context &ctx, Mal_Proc_Args args)
{
    auto thisptr = static_cast<Mal_Integer*>(args[0]);
    auto other = static_cast<Mal_Integer*>(args[1]);
    return new Mal_Integer(thisptr->value % other->value);
}

void Mal_Integer::register_type()
{
    static bool registered = false;
    if (registered)
    {
        return;
    }
    auto t = Type::get_or_create_type("Integer");

    t->methods()[Method_to_string] = int_to_string;
    t->methods()[Method_inspect] = int_inspect;
    t->methods()[Method_b_op_add] = int_b_op_add;
    t->methods()[Method_b_op_sub] = int_b_op_sub;
    t->methods()[Method_b_op_mul] = int_b_op_mul;
    t->methods()[Method_b_op_div] = int_b_op_div;
    t->methods()[Method_b_op_mod] = int_b_op_mod;

    t->define_type(Type::get_or_create_type("Object"));

    registered = true;
}
