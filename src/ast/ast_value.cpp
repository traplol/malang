#include <sstream>
#include "ast_value.hpp"



Integer_Node::~Integer_Node()
{
    PRINT_DTOR;
}
std::string Integer_Node::to_string() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Integer_Node)
{
    printf("Executed %s (%ld)\n", node_name().c_str(), value);
    return this;
}



Real_Node::~Real_Node()
{
    PRINT_DTOR;
}
std::string Real_Node::to_string() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Real_Node)
{
    printf("Executed %s (%f)\n", node_name().c_str(), value);
    return this;
}



String_Node::~String_Node()
{
    PRINT_DTOR;
}
std::string String_Node::to_string() const
{
    std::stringstream ss;
    ss << "\"" << value << "\"";
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(String_Node)
{
    printf("Executed %s (%s)\n", node_name().c_str(), value.data());
    return this;
}




Boolean_Node::~Boolean_Node()
{
    PRINT_DTOR;
}
std::string Boolean_Node::to_string() const
{
    return value ? "true" : "false";
}
AST_NODE_OVERRIDES_IMPL(Boolean_Node)
{
    printf("Executed %s (%s)\n", node_name().c_str(), value ? "true" : "false");
    return this;
}



Reference_Node::~Reference_Node()
{
    PRINT_DTOR;
}
std::string Reference_Node::to_string() const
{
    std::stringstream ss;
    ss << "Reference_To(" << value << ")";
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Reference_Node)
{
    Ast_Node *result = nullptr;
    if (value)
    {
        result = value->execute(ctx);
    }
    printf("Executed %s (Ref->%p)\n", node_name().c_str(), value);
    return result;
}
