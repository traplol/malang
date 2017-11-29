#include <sstream>
#include "ast_variable.hpp"

Variable_Node::~Variable_Node()
{
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Variable_Node)

const std::string &Variable_Node::name()
{
    if (!m_full_name.empty())
    {
        return m_full_name;
    }
    std::stringstream ss;
    for (auto &&q : m_qualifiers)
    {
        ss << q << "::";
    }
    ss << m_name;
    m_full_name = ss.str();
    return m_full_name;
}
