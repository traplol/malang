#ifndef MALANG_IR_IR_HPP
#define MALANG_IR_IR_HPP

#include <vector>
#include "../metadata.hpp"
#include "../type_map.hpp"
#include "../source_code.hpp"
#include "ir_visitor.hpp"
#include "label_map.hpp"

#define IR_NODE_OVERRIDES \
    METADATA_OVERRIDES; \
    virtual void accept(IR_Visitor&) override

#define IR_NODE_OVERRIDES_IMPL(class_name) \
    METADATA_OVERRIDES_IMPL(class_name) \
    void class_name::accept(IR_Visitor &visitor) { visitor.visit(*this); }

struct IR_Node : public Metadata
{
    IR_Node(const Source_Location &src_loc)
        : src_loc(src_loc)
        {}
    virtual ~IR_Node(){}
    virtual void accept(IR_Visitor&) = 0;
    METADATA_OVERRIDES;

    Source_Location src_loc;
protected:
    friend struct Malang_IR;
    //void * operator new(size_t);
    //void operator delete(void *);
};

struct Malang_IR
{
    ~Malang_IR();
    Malang_IR(Type_Map *types)
        : types(types)
        , labels(new Label_Map{this})
        {}
    Type_Map *types;
    Label_Map *labels;
    std::vector<IR_Node*> first;
    std::vector<IR_Node*> second;

    // This scheme has been adopted because some IR_Nodes are shared (e.g labels) as the
    // base type IR_Node* which leads to a horrible ownership problem. The simplest solution
    // is to say no IR_Node owns another IR_Node only the Malang_IR container that allocated
    // it has ownership. This also makes sense because realisticly any leaf node depended on
    // by its parent, and that node's parent, and so forth to the root node.
    template <typename T, typename... Args>
    T *alloc(Args&&... args)
    {
        T *t = new T(std::forward<Args>(args)...);
        m_own_allocated_nodes.push_back(t);
        return t;
    }
private:
    std::vector<IR_Node*> m_own_allocated_nodes;
};

#endif /* MALANG_IR_IR_HPP */
