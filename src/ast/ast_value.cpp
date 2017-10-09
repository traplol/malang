#include <sstream>
#include "ast_value.hpp"



Integer_Node::~Integer_Node()
{
    PRINT_DTOR;
}
std::string Integer_Node::to_string() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
METADATA_OVERRIDES_IMPL(Integer_Node)




Real_Node::~Real_Node()
{
    PRINT_DTOR;
}
std::string Real_Node::to_string() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
METADATA_OVERRIDES_IMPL(Real_Node)




String_Node::~String_Node()
{
    PRINT_DTOR;
}
std::string String_Node::to_string() const
{
    std::stringstream ss;
    ss << "\"" << value << "\"";
    return ss.str();
}
METADATA_OVERRIDES_IMPL(String_Node)





Boolean_Node::~Boolean_Node()
{
    PRINT_DTOR;
}
std::string Boolean_Node::to_string() const
{
    return value ? "true" : "false";
}
METADATA_OVERRIDES_IMPL(Boolean_Node)




Reference_Node::~Reference_Node()
{
    PRINT_DTOR;
}
std::string Reference_Node::to_string() const
{
    std::stringstream ss;
    ss << "Reference_To(" << value << ")";
    return ss.str();
}
METADATA_OVERRIDES_IMPL(Reference_Node)

