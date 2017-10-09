#include <sstream>
#include "obj_boolean.hpp"

std::string Mal_Boolean::to_string() const
{
    return value ? "true" : "false";
}
std::string Mal_Boolean::inspect() const
{
    return value ? "true" : "false";
}
Type *Mal_Boolean::type() const
{
    return Type::get_type("Boolean");
}
