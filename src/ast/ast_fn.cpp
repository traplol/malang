#include <stdio.h>
#include <sstream>

#include "ast_fn.hpp"

Fn_Node::~Fn_Node()
{
    for (auto &&p : params)
    {
        delete p;
    }
    params.clear();
    delete return_type;
    return_type = nullptr;
    for (auto &&n : body)
    {
        delete n;
    }
    body.clear();
    PRINT_DTOR;
}
Type_Info *Fn_Node::get_type()
{
    return fn_type;
}
AST_NODE_OVERRIDES_IMPL(Fn_Node);


