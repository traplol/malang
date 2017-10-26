#include "ast_loop.hpp"

While_Node::~While_Node()
{
    delete condition;
    condition = nullptr;
    for (auto &&n : body)
    {
        delete n;
    }
    body.clear();
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(While_Node);
