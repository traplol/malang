#include <sstream>
#include "obj_string.hpp"

std::string Mal_String::to_string() const
{
    return value;
}
std::string Mal_String::inspect() const
{
    return value;
}
Type *Mal_String::type() const
{
    return Type::get_type("String");
}
