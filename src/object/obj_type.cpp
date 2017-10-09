#include <sstream>
#include "obj_type.hpp"

std::string Mal_Type::to_string() const
{
    return value->name();
}
std::string Mal_Type::inspect() const
{
    return value->name();
}
Type *Mal_Type::type() const
{
    return Type::get_type("Type");
}
