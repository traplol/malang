#include "ast_type_def.hpp"
#include "ast_fn.hpp"
#include "ast_decl.hpp"

AST_NODE_OVERRIDES_IMPL(Constructor_Node);
Constructor_Node::~Constructor_Node()
{
    for (auto &&p : params)
    {
        delete p;
    }
    params.clear();
    for (auto &&n : body)
    {
        delete n;
    }
    body.clear();
    PRINT_DTOR;
}
AST_NODE_OVERRIDES_IMPL(Type_Def_Node);
Type_Def_Node::~Type_Def_Node()
{
    for (auto &ctor : constructors)
    {
        delete ctor;
    }
    constructors.clear();
    for (auto &method : methods)
    {
        delete method;
    }
    methods.clear();
    for (auto &field : fields)
    {
        delete field;
    }
    fields.clear();
}

