#include "ir_member_access.hpp"

IR_NODE_OVERRIDES_IMPL(IR_Member_Access);

Type_Info *IR_Member_Access::get_type() const
{
    auto thing_ty = thing->get_type();
    assert(thing_ty);

    // @FixMe: make this work with function overloading?...
    // I'm still unsure if types should be capable of binding the same name to multiple types.
    // Doing so makes type inference very hairy and difficult to select a function member
    // without passing argument information.
    // One solution to this may be to add a special case when calling a member that handles
    // the function lookup there. This whould be factored out and usable when searching for
    // hard-defined functions.

    auto field = thing_ty->get_field(member_name);
    if (!field)
    {
        src_loc.report("error", "type `%s' does not have a member called `%s'",
                       thing_ty->name().c_str(), member_name.c_str());
        abort();
    }
    return field->type();
}
