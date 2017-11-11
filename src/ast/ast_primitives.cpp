#include <sstream>
#include "ast_primitives.hpp"


Integer_Node::~Integer_Node()
{
    PRINT_DTOR;
}
Type_Info *Integer_Node::get_type()
{
    return type;
}
AST_NODE_OVERRIDES_IMPL(Integer_Node)

Real_Node::~Real_Node()
{
    PRINT_DTOR;
}
Type_Info *Real_Node::get_type()
{
    return type;
}
AST_NODE_OVERRIDES_IMPL(Real_Node)

String_Node::~String_Node()
{
    PRINT_DTOR;
}
Type_Info *String_Node::get_type()
{
    return type;
}
AST_NODE_OVERRIDES_IMPL(String_Node)

Boolean_Node::~Boolean_Node()
{
    PRINT_DTOR;
}
Type_Info *Boolean_Node::get_type()
{
    return type;
}
AST_NODE_OVERRIDES_IMPL(Boolean_Node)

Character_Node::~Character_Node()
{
    PRINT_DTOR;
}
Type_Info *Character_Node::get_type()
{
    return type;
}
AST_NODE_OVERRIDES_IMPL(Character_Node)
