#include <stdio.h>
#include "../vm.hpp"
#include "../../type_map.hpp"
#include "gc.hpp"

GC_List::~GC_List()
{
}

Malang_GC::~Malang_GC()
{
    printf("TODO: Cleanup GC\n");
}

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
        if ((a)[i].is_object()) {              \
            marked++;                           \
            (a)[i].as_object()->gc_mark();}}

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
        if (cur->u.object.header.color == Malang_Object::white)
        {
            free(cur);
            ++freed;
        }
        else
        {
            cur->u.object.header.color = Malang_Object::white;
        }
        cur = next;
        ++visited;
    }
    printf("GC sweep: visited: %ld freed: %ld\n", visited, freed);
}

void Malang_GC::construct_object(Malang_Object_Body &obj, Type_Info *type)
{
    assert(type);
    obj.header.type = type;
    obj.header.allocator = this;
    obj.header.free = false;
    obj.header.is_array = false;
    // don't want to immediately free this object...
    obj.header.color = Malang_Object::grey;
    // @TODO: reserve fields and such 
}

void Malang_GC::construct_array(Malang_Array &arr, Type_Info *type, Fixnum length)
{
    assert(type);
    arr.header.type = type;
    arr.header.allocator = this;
    arr.header.free = false;
    arr.header.is_array = true;
    // don't want to immediately free this array...
    arr.header.color = Malang_Object::grey;
    arr.size = length;
    if (length)
    {
        // @FixMe: should initialization be handled? maybe call ctor for every element
        arr.data = new Malang_Value[length];
    }
    else
    {
        arr.data = nullptr;
    }
}

GC_Node *Malang_GC::alloc_intern()
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
    return gc_node;
}

Malang_Object *Malang_GC::allocate_object(Type_Token type_token)
{
    auto gc_node = alloc_intern();
    auto type = m_types->get_type(type_token);
    construct_object(gc_node->u.object, type);
    m_allocated.append(gc_node);
    m_num_allocated++;
    return &(gc_node->u.object.header);
}

Malang_Object *Malang_GC::allocate_array(Type_Token of_type_token, Fixnum length)
{
    auto gc_node = alloc_intern();
    auto type = m_types->get_type(of_type_token);
    construct_array(gc_node->u.array, type, length);
    m_allocated.append(gc_node);
    m_num_allocated++;
    return &(gc_node->u.array.header);
}

void Malang_GC::free_object(Malang_Object_Body *obj)
{
    if (obj->header.free)
    {
        return;
    }
    if (obj->fields)
    {
        delete[] obj->fields;
        obj->fields = nullptr;
    }
}
void Malang_GC::free_array(Malang_Array *arr)
{
    if (arr->header.free)
    {
        return;
    }
    if (arr->data)
    {
        delete[] arr->data;
        arr->data = nullptr;
    }
    arr->size = 0;
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
    assert(obj->free == false);
    if (obj->is_array)
    {
        free_array(reinterpret_cast<Malang_Array*>(obj));
    }
    else
    {
        free_object(reinterpret_cast<Malang_Object_Body*>(obj));
    }
    obj->free = true;
    auto ptr = reinterpret_cast<uintptr_t>(obj);
    ptr -= offsetof(GC_Node, u);

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
