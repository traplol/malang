#ifndef MALANG_VM_GC_HPP
#define MALANG_VM_GC_HPP

#include "object.hpp"

struct GC_Node;
struct GC_List
{
    ~GC_List();
    std::vector<GC_Node*> nodes;
    void append(GC_Node *node);
    void remove(GC_Node *node);
    GC_Node *pop();
};

struct Malang_GC
{
    ~Malang_GC();
    Malang_GC(struct Malang_VM *vm, struct Type_Map *types, size_t run_interval, size_t max_objects)
        : m_is_paused(false)
        , m_vm(vm)
        , m_types(types)
        , m_total_allocated(0)
        , m_total_freed(0)
        , m_next_run(run_interval)
        , m_run_interval(run_interval)
        , m_max_objects(max_objects)
        {}

    void disable_automatic();
    void enable_automatic();
    void manual_run();
    void deallocate(Malang_Object *obj);
    Malang_Object *allocate_object(Type_Token type_token);
    Malang_Object *allocate_array(Type_Token of_type_token, Fixnum length);
private:
    GC_Node *alloc_intern();
    void free_node(struct GC_Node *gc_node);
    void free_object(Malang_Object *obj);
    void free_object_body(Malang_Object_Body *obj);
    void free_array(Malang_Array *arr);
    void construct_object(Malang_Object_Body &obj, Type_Info *type);
    void construct_array(Malang_Array &arr, Type_Info *of_type, Fixnum length);
    void mark();
    void sweep();
    void mark_and_sweep();
    bool m_is_paused;
    struct Malang_VM *m_vm;
    struct Type_Map *m_types;
    size_t m_total_allocated;
    size_t m_total_freed;
    size_t m_next_run;
    size_t m_run_interval;
    size_t m_max_objects;
    GC_List m_allocated;
    GC_List m_free;
};


#endif /* MALANG_VM_GC_HPP */
