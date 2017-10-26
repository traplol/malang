#include "ast_if_else.hpp"

AST_NODE_OVERRIDES_IMPL(If_Else_Node)

If_Else_Node::~If_Else_Node()
{
    delete condition;
    condition = nullptr;
    for (auto &&c : consequence)
    {
        delete c;
    }
    consequence.clear();
    for (auto &&a : alternative)
    {
        delete a;
    }
    alternative.clear();
}

Type_Info *If_Else_Node::get_type()
{
    // This node's type is deduced by the following:
    //   0. If either leg is empty, the type is void
    //   1. Otherwise, both legs must have an Ast_Value as the last node of both legs
    //      if these differ, an error should be thrown because get_type won't be called
    //      in any other circumstances other than wanting the deduced type!
    //   2. Because either leg may contain another If_Else_Node special care must be
    //      taken when a leg's type return's void
    //   3. The alternative's type must be assignable to the consequence's type.
    if (consequence.empty() || alternative.empty())
    {
        return void_type;
    }
    auto last_conseq = dynamic_cast<Ast_Value*>(consequence.back());
    auto last_altern = dynamic_cast<Ast_Value*>(alternative.back());
    if (!last_conseq && !last_altern)
    {
        return void_type;
    }
    if (!last_conseq)
    {
        auto alt_ty = last_altern->get_type();
        assert(alt_ty);
        src_loc.report("error", "Expected consequence leg of if/else to be type `%s'",
                         alt_ty->name().c_str());
        abort();
    }
    if (!last_altern)
    {
        auto con_ty = last_conseq->get_type();
        assert(con_ty);
        src_loc.report("error", "Expected alternative leg of if/else to be type `%s'",
                         con_ty->name().c_str());
        abort();
    }

    auto alt_ty = last_altern->get_type();
    auto con_ty = last_altern->get_type();
    // Handle the case where the alternative leg is another If_Else_Node that doesn't
    // resolve to a type.
    if (!alt_ty && con_ty) 
    {
        src_loc.report("error", "Expected alternative leg of if/else to be type `%s'",
                         con_ty->name().c_str());
        abort();
    }
    // Handle the case where the consequence leg is another If_Else_Node that doesn't
    // resolve to a type.
    if (alt_ty && !con_ty) 
    {
        src_loc.report("error", "Expected consequence leg of if/else to be type `%s'",
                         alt_ty->name().c_str());
        abort();
    }

    if (alt_ty->is_assignable_to(con_ty))
    {
        return con_ty;
    }
    src_loc.report("error", "Cannot convert from type `%s' to `%s'",
                   alt_ty->name().c_str(), con_ty->name().c_str());
    abort();
}
