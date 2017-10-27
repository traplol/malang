#include <stdio.h>
#include <sstream>
#include "ast.hpp"

Ast_Node::~Ast_Node()
{
    PRINT_DTOR;
}

METADATA_OVERRIDES_IMPL(Ast_Node)

Ast::~Ast()
{
    for (auto &&n : roots)
    {
        delete n;
    }
    roots.clear();
}
