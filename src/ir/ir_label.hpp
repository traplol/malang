#ifndef MALANG_IR_IR_LABEL_HPP
#define MALANG_IR_IR_LABEL_HPP

#include <string>
#include <vector>
#include "ir.hpp"

struct Backfill_On_Resolve
{
    struct Codegen *cg;
    size_t index;
    size_t from;
    bool wants_relative;
};

struct IR_Label : IR_Node
{
    IR_Label(const Source_Location &src_loc, const std::string &name)
        : IR_Node(src_loc)
        , m_name(name)
        , m_is_resolved(false)
        , m_local_address(0)
        {}

    IR_NODE_OVERRIDES;

    const std::string &name() const;
    bool is_resolved() const;
    int32_t address() const;
    void address(int32_t addr);
    void please_backfill_on_resolve(struct Codegen *cg, size_t index);
    void please_backfill_on_resolve_rel(struct Codegen *cg, size_t index, size_t from);

private:
    std::string m_name;
    bool m_is_resolved;
    int32_t m_local_address;
    std::vector<Backfill_On_Resolve> m_to_be_backfilled;
};

struct IR_Named_Block : IR_Label
{
    IR_Named_Block(const Source_Location &src_loc, const std::string &name, IR_Label *end)
        : IR_Label(src_loc, name)
        , m_end(end)
        {}

    IR_NODE_OVERRIDES;

    std::vector<IR_Node*> &body();
    IR_Label *end() const;

private:
    IR_Label *m_end;
    std::vector<IR_Node*> m_body;

};

#endif /* MALANG_IR_IR_LABEL_HPP */
