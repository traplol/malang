#include <stdio.h>
#include <sstream>
#include "ast.hpp"

Ast_Node::~Ast_Node()
{
    PRINT_DTOR;
}

std::string Ast_Node::to_string() const
{
    return "<BASE Ast_Node>";
}

METADATA_OVERRIDES_IMPL(Ast_Node)

std::string Ast::to_string() const
{
    std::stringstream ss;
    for (size_t i = 0; i < roots.size(); ++i)
    {
        ss << roots[i]->to_string();
        if (i+1 < roots.size())
        {
            ss << std::endl;
        }
    }
    return ss.str();
}
