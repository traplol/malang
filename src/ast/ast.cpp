#include <stdio.h>
#include <sstream>
#include "ast.hpp"
#include "nodes.hpp"

Ast_Node::~Ast_Node()
{
    PRINT_DTOR;
}

METADATA_OVERRIDES_IMPL(Ast_Node)

Ast::~Ast()
{
    for (auto &&n : imports)
    {
        delete n;
    }
    imports.clear();

    //for (auto &&n : type_defs)
    //{
    //    delete n;
    //}
    //type_defs.clear();

    //for (auto &&n : extensions)
    //{
    //    delete n;
    //}
    //extensions.clear();

    //for (auto &&n : bound_funcs)
    //{
    //    delete n;
    //}
    //bound_funcs.clear();

    for (auto &&n : first)
    {
        delete n;
    }
    first.clear();
    for (auto &&n : second)
    {
        delete n;
    }
    second.clear();
}
