#include "ast_list.hpp"
#include "ast_return.hpp"

Return_Node::~Return_Node()
{
    delete values;
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Return_Node);
