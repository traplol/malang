#include <sstream>
#include "ast_list.hpp"

List_Node::~List_Node()
{
    for (auto &&p : contents)
    {
        delete p;
    }
    contents.clear();
    PRINT_DTOR;
}
std::string List_Node::to_string() const
{
    std::stringstream ss;
    for (size_t i = 0; i < contents.size(); ++i)
    {
        ss << contents[i]->to_string();
        if (i+1 < contents.size())
        {
            ss << ", ";
        }
    }
    return ss.str();
}
AST_NODE_OVERRIDES_IMPL(List_Node)
{
    printf("Executed %s [%s]\n", node_name().c_str(), to_string().c_str());
    return this;
}
