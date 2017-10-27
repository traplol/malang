#include "ast_array.hpp"
#include "ast_type.hpp"

AST_NODE_OVERRIDES_IMPL(Array_Literal_Node);
AST_NODE_OVERRIDES_IMPL(New_Array_Node);

Array_Literal_Node::~Array_Literal_Node()
{
    delete values;
    values = nullptr;
    PRINT_DTOR;
}

Type_Info *Array_Literal_Node::get_type()
{
    // figure it out at AST->IR translation
    return nullptr;
}

New_Array_Node::~New_Array_Node()
{
    array_type = nullptr;
    of_type = nullptr;
    delete size;
    size = nullptr;
    PRINT_DTOR;
}

Type_Info *New_Array_Node::get_type()
{
    return array_type;
}
