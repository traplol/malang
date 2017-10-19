#ifndef MALANG_VM_GC_HPP
#define MALANG_VM_GC_HPP

#include "object.hpp"

struct GC_Node
{
    GC_Node *next, *prev;
    Malang_Object obj;
};

struct GC_List
{
    GC_List()
        : head(nullptr)
        , tail(nullptr)
        {}
    GC_Node *head;
    GC_Node *tail;
    void append(GC_Node *node);
    void remove(GC_Node *node);
    GC_Node *pop();
};

struct Malang_GC
{
    Malang_GC(struct Malang_VM *vm, size_t run_interval)
        : m_is_paused(false)
        , m_vm(vm)
        , m_num_allocated(0)
        , m_next_run(run_interval)
        , m_run_interval(run_interval)
        {}

    void disable_automatic();
    void enable_automatic();
    void manual_run();
    void free(Malang_Object *obj);
    Malang_Object *allocate(Type_Info *type);
private:
    void free(GC_Node *gc_node);
    void construct(Malang_Object &obj, Type_Info *type);
    void mark();
    void sweep();
    void mark_and_sweep();
    bool m_is_paused;
    struct Malang_VM *m_vm;
    size_t m_num_allocated;
    size_t m_next_run;
    size_t m_run_interval;
    GC_List m_allocated;
    GC_List m_free;
};


#endif /* MALANG_VM_GC_HPP */
