#ifndef MALANG_IR_IR_HPP
#define MALANG_IR_IR_HPP

#include <vector>
#include "../metadata.hpp"
#include "ir_visitor.hpp"
#include "type_map.hpp"
#include "label_map.hpp"
#include "symbol_map.hpp"

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

struct Malang_IR
{
    std::vector<IR_Node*> roots;
    Symbol_Map symbols;
    Label_Map labels;
    Type_Map types;
};

#endif /* MALANG_IR_IR_HPP */
