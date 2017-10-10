#include <stdio.h>
#include <sstream>
#include "ast_assign.hpp"

Assign_Node::~Assign_Node()
{
    delete lhs;
    lhs = nullptr;
    delete rhs;
    rhs = nullptr;
    PRINT_DTOR;
}

AST_NODE_OVERRIDES_IMPL(Assign_Node)

