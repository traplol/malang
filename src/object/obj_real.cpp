#include <sstream>
#include "obj_real.hpp"

std::string Mal_Real::to_string() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
std::string Mal_Real::inspect() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
Type *Mal_Real::type() const
{
    return Type::get_type("Real");
}
