#include <stdio.h>
#include <sstream>
#include "ast_decl.hpp"

Decl_Node::~Decl_Node()
{
    delete type;
    type = nullptr;
    PRINT_DTOR;
}

Type_Info *Decl_Node::get_type()
{
    if (type)
    {
        return type->type;
    }
    return nullptr;
}

AST_NODE_OVERRIDES_IMPL(Decl_Node);


Decl_Assign_Node::~Decl_Assign_Node()
{
    delete decl;
    decl = nullptr;
    delete value;
    value = nullptr;
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Decl_Assign_Node);
