#include <stdio.h>
#include "vm.hpp"
#include "gc.hpp"

#define offsetof(a,b) __builtin_offsetof(a,b)

void Malang_GC::disable_automatic()
{
    printf("GC: pausing\n");
    m_is_paused = true;
}

void Malang_GC::enable_automatic()
{
    printf("GC: resuming\n");
    m_is_paused = false;
}

void Malang_GC::manual_run()
{
    printf("GC: manual run\n");
    mark_and_sweep();
}

void Malang_GC::mark_and_sweep()
{
    mark();
    sweep();
}

void Malang_GC::mark()
{
    assert(m_vm);
    size_t visited = 0, marked = 0;
#define _mark(n, a)                             \
    for (uintptr_t i = 0; i < (n); ++i) {       \
        visited++;                              \
        if ((a)[i].is_pointer()) {              \
            marked++;                           \
            (a)[i].as_pointer()->gc_mark();}}

    printf("GC: allocated: %ld\n", m_num_allocated);
    printf("GC: marking globals\n");
    _mark(m_vm->globals_top, m_vm->globals);
    printf("GC: marking locals\n");
    _mark(m_vm->locals_top, m_vm->locals);
    printf("GC: marking data stack\n");
    _mark(m_vm->data_top, m_vm->data_stack);

    printf("GC mark: visited: %ld marked: %ld\n", visited, marked);
#undef _mark

}

void Malang_GC::sweep()
{
    assert(m_vm);
    auto cur = m_allocated.head;
    size_t visited = 0, freed = 0;
    while (cur)
    {
        auto next = cur->next;
        if (cur->obj.color == Malang_Object::white)
        {
            free(cur);
            ++freed;
        }
        else
        {
            cur->obj.color = Malang_Object::white;
        }
        cur = next;
        ++visited;
    }
    printf("GC sweep: visited: %ld freed: %ld\n", visited, freed);
}

void Malang_GC::construct(Malang_Object &obj, Type_Info *type)
{
    assert(type);
    obj.allocator = this;
    obj.free = false;
    obj.type = type;
    // don't want to immediately free this object...
    obj.color = Malang_Object::grey;
    // TODO: reserve fields and such 
}

Malang_Object *Malang_GC::allocate(Type_Info *type)
{
    if (!m_is_paused && m_num_allocated > m_next_run)
    {
        m_next_run += m_run_interval;
        printf("GC: automatic run triggered\n");
        mark_and_sweep();
    }
    auto gc_node = m_free.pop();
    if (!gc_node)
    {
        gc_node = new GC_Node;
        gc_node->next = nullptr;
    }
    construct(gc_node->obj, type);
    m_allocated.append(gc_node);
    m_num_allocated++;
    return &(gc_node->obj);
}

void Malang_GC::free(GC_Node *gc_node)
{
    m_allocated.remove(gc_node);
    m_free.append(gc_node);
    m_num_allocated--;
}

void Malang_GC::free(Malang_Object *obj)
{
    assert(obj->allocator == this);
    if (obj->free) return;
    obj->free = true;
    auto ptr = reinterpret_cast<uintptr_t>(obj);
    ptr -= offsetof(GC_Node, obj);

    free(reinterpret_cast<GC_Node*>(ptr));
}

void GC_List::append(GC_Node *node)
{
    assert(node);
    if (!head)
    {
        head = node;
        tail = head;
    }
    else
    {
        tail->next = node;
        node->prev = tail;
        node->next = nullptr;
        tail = node;
    }
}

void GC_List::remove(GC_Node *node)
{
    assert(node);
    if (node == head)
    {
        head = head->next;
        if (head)
            head->prev = nullptr;
    }
    if (node == tail)
    {
        tail = tail->prev;
        if (tail)
            tail->next = nullptr;
    }
    if (node->prev)
    {
        node->prev->next = node->next;
    }
    if (node->next)
    {
        node->next->prev = node->prev;
    }
    node->prev = nullptr;
    node->next = nullptr;
}

GC_Node *GC_List::pop()
{
    if (!tail)
    {
        return nullptr;
    }
    auto gc_node = tail;
    remove(tail);
    gc_node->prev = nullptr;
    gc_node->next = nullptr;
    return gc_node;
}
