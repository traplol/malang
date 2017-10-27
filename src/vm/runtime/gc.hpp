#ifndef MALANG_VM_GC_HPP
#define MALANG_VM_GC_HPP

#include "object.hpp"

#define offsetof(a,b) __builtin_offsetof(a,b)

struct GC_Node
{
    GC_Node() : next(nullptr), prev(nullptr) {}
    GC_Node *next, *prev;
    union {
        Malang_Object_Body object;
        Malang_Array array;
    } u;
};

static_assert(offsetof(GC_Node, u.object.header) == offsetof(GC_Node, u.array.header),
              "object and array headers are not at the same offset!");

struct GC_List
{
    ~GC_List();
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
    ~Malang_GC();
    Malang_GC(struct Malang_VM *vm, struct Type_Map *types, size_t run_interval)
        : m_is_paused(false)
        , m_vm(vm)
        , m_types(types)
        , m_num_allocated(0)
        , m_next_run(run_interval)
        , m_run_interval(run_interval)
        {}

    void disable_automatic();
    void enable_automatic();
    void manual_run();
    void free(Malang_Object *obj);
    Malang_Object *allocate_object(Type_Token type_token);
    Malang_Object *allocate_array(Type_Token of_type_token, Fixnum length);
private:
    GC_Node *alloc_intern();
    void free(GC_Node *gc_node);
    void free_object(Malang_Object_Body *obj);
    void free_array(Malang_Array *arr);
    void construct_object(Malang_Object_Body &obj, Type_Info *type);
    void construct_array(Malang_Array &arr, Type_Info *of_type, Fixnum length);
    void mark();
    void sweep();
    void mark_and_sweep();
    bool m_is_paused;
    struct Malang_VM *m_vm;
    struct Type_Map *m_types;
    size_t m_num_allocated;
    size_t m_next_run;
    size_t m_run_interval;
    GC_List m_allocated;
    GC_List m_free;
};


#endif /* MALANG_VM_GC_HPP */
