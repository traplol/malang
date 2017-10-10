#include <sstream>
#include "ast_value.hpp"



Integer_Node::~Integer_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Integer_Node)




Real_Node::~Real_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Real_Node)




String_Node::~String_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(String_Node)





Boolean_Node::~Boolean_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Boolean_Node)




Reference_Node::~Reference_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Reference_Node)

