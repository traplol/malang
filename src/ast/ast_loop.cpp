#include "ast_loop.hpp"

AST_NODE_OVERRIDES_IMPL(While_Node);
AST_NODE_OVERRIDES_IMPL(For_Node);

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

For_Node::~For_Node()
{
    delete iterable;
    iterable = nullptr;
    for (auto &&n : body)
    {
        delete n;
    }
    body.clear();
    PRINT_DTOR;
}
