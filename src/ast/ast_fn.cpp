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
std::string Fn_Node::to_string() const
{
    std::stringstream ss;
    ss << "fn (";
    for (size_t i = 0; i < params.size(); ++i)
    {
        ss << params[i]->to_string();
        if (i + 1 < params.size())
        {
            ss << ", ";
        }
    }
    ss << ") -> " << return_type->to_string() << " {" << std::endl;
    for (auto &&n : body)
    {
        ss << "    " << n->to_string() << std::endl;
    }
    ss << "}";
    
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Fn_Node);


