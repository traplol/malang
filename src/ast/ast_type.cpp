#include "ast_type.hpp"

Type_Node::~Type_Node()
{
    // do not delete the type; the Type_Map owns that.
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Type_Node);
