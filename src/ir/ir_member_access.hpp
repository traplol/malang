#ifndef MALANG_IR_IR_MEMBER_ACCESS_HPP
#define MALANG_IR_IR_MEMBER_ACCESS_HPP

#include "ir_values.hpp"

struct IR_Member_Access : IR_LValue
{
    virtual ~IR_Member_Access() = default;
    IR_Member_Access(const Source_Location &src_loc, IR_Value *thing, const std::string &member_name)
        : IR_LValue(src_loc)
        , thing(thing)
        , member_name(member_name)
        {}

    IR_NODE_OVERRIDES;

    virtual Type_Info *get_type() const override;

    IR_Value *thing;
    std::string member_name;
};

#endif /* MALANG_IR_IR_MEMBER_ACCESS_HPP */
