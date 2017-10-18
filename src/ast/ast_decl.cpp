#include <stdio.h>
#include <sstream>
#include "ast_decl.hpp"

Decl_Node::~Decl_Node()
{
    delete type;
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
    delete value;
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Decl_Assign_Node);

Decl_Constant_Node::~Decl_Constant_Node()
{
    delete decl;
    delete value;
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Decl_Constant_Node);
