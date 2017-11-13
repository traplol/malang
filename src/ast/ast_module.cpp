#include "ast_module.hpp"

Import_Node::~Import_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Import_Node);
