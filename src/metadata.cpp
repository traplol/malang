#include <sstream>

#include "metadata.hpp"

std::string Metadata::to_string() const
{
    std::stringstream ss;
    ss << type_name() << "#" << this;
    return ss.str();
}
