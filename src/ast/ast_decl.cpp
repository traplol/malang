#include <stdio.h>
#include <sstream>
#include "ast_decl.hpp"

Decl_Node::~Decl_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Decl_Node);
