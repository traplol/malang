#include <stdio.h>
#include "ast.hpp"

Node_Type_Id Ast_Node::num_node_types = 0;

Ast_Node::~Ast_Node()
{
    PRINT_DTOR;
}

std::string Ast_Node::to_string() const
{
    return "<BASE Ast_Node>";
}

AST_NODE_OVERRIDES_IMPL(Ast_Node)
{
    printf("Executed %s!\n", node_name().c_str());
    return nullptr;
}
