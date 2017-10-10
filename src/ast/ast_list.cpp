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
AST_NODE_OVERRIDES_IMPL(List_Node);
