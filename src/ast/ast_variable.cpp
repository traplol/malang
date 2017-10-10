#include "ast_variable.hpp"

Variable_Node::~Variable_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Variable_Node)

