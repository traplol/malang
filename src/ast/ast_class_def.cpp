#include "ast_class_def.hpp"
#include "ast_fn.hpp"
#include "ast_decl.hpp"

AST_NODE_OVERRIDES_IMPL(Class_Def_Node);
Class_Def_Node::~Class_Def_Node()
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
