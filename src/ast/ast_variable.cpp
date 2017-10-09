#include "ast_variable.hpp"

Variable_Node::~Variable_Node()
{
    PRINT_DTOR;
}
std::string Variable_Node::to_string() const
{
    return name;
}
METADATA_OVERRIDES_IMPL(Variable_Node)

