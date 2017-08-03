#include <stdio.h>
#include <sstream>
#include "ast_assign.hpp"

Assign_Node::~Assign_Node()
{
    delete lhs;
    lhs = nullptr;
    delete rhs;
    rhs = nullptr;
    PRINT_DTOR;
}
std::string Assign_Node::to_string() const
{
    std::stringstream ss;
    ss << lhs->to_string() << " = " << rhs->to_string();
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(Assign_Node)
{
    lhs->execute(ctx);
    rhs->execute(ctx);
    printf("Executed %s\n", node_name().c_str());
    return nullptr;
}
