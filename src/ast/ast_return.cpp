#include "ast_list.hpp"
#include "ast_return.hpp"

AST_NODE_OVERRIDES_IMPL(Return_Node);
AST_NODE_OVERRIDES_IMPL(Break_Node);
AST_NODE_OVERRIDES_IMPL(Continue_Node);

Return_Node::~Return_Node()
{
    delete values;
    PRINT_DTOR;
}
Break_Node::~Break_Node()
{
    delete values;
    PRINT_DTOR;
}
Continue_Node::~Continue_Node()
{
    PRINT_DTOR;
}
