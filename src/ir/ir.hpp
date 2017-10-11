#ifndef MALANG_IR_IR_HPP
#define MALANG_IR_IR_HPP

#include "../metadata.hpp"
#include "ir_visitor.hpp"

#define IR_NODE_OVERRIDES \
    METADATA_OVERRIDES; \
    virtual void accept(IR_Visitor&) override

#define IR_NODE_OVERRIDES_IMPL(class_name) \
    METADATA_OVERRIDES_IMPL(class_name) \
    void class_name::accept(IR_Visitor &visitor) { visitor.visit(*this); }

struct IR_Node : public Metadata
{
    virtual ~IR_Node(){}
    virtual void accept(IR_Visitor&) = 0;
    METADATA_OVERRIDES;
};

#endif /* MALANG_IR_IR_HPP */
