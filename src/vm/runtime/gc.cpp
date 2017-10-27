#include <stdio.h>
#include "../vm.hpp"
#include "../../type_map.hpp"
#include "gc.hpp"

#define panic(...) { printf(__VA_ARGS__); abort(); }
#define offsetof(a,b) __builtin_offsetof(a,b)

struct GC_Node
{
    GC_Node(struct GC_List *magic_number)
        : magic(magic_number)
        , lookup_index(magic_index) {}
    struct GC_List *magic;
    size_t lookup_index;
    union _ {
        Malang_Object_Body object;
        Malang_Array array;
    } u;
    static constexpr decltype(lookup_index) magic_index = static_cast<decltype(lookup_index)>(-1);
};

static_assert(offsetof(GC_Node, u.object.header) == offsetof(GC_Node, u.array.header),
              "object and array headers are not at the same offset!");

GC_List::~GC_List()
{
    for (auto &&node : nodes)
    {
        assert(node->magic == this);
        delete node;
    }
    nodes.clear();
}

Malang_GC::~Malang_GC()
{
    printf("available: %ld\n", m_free.nodes.size());
    printf("allocated: %ld\n", m_allocated.nodes.size());
    sweep();
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
    printf("GC: magic free     : %p\n", &m_free);
    printf("GC: magic allocated: %p\n", &m_allocated);
    size_t visited = 0, reachable = 0;
#define _mark(n, a)                             \
    for (uintptr_t i = 0; i < (n); ++i) {       \
        visited++;                              \
        if ((a)[i].is_object()) {              \
            reachable++;                           \
            (a)[i].as_object()->gc_mark();}}

    printf("GC: in use: %ld available: %ld\n", m_allocated.nodes.size(), m_free.nodes.size());
    printf("GC: total allocated: %ld freed:%ld\n", m_total_allocated, m_total_freed);
    _mark(m_vm->globals_top, m_vm->globals);
    _mark(m_vm->locals_top, m_vm->locals);
    _mark(m_vm->data_top, m_vm->data_stack);
    printf("GC mark: visited: %ld reachable: %ld\n", visited, reachable);
#undef _mark

}

void Malang_GC::sweep()
{
    assert(m_vm);
    size_t visited = 0, freed = 0;
    for (auto &&cur : m_allocated.nodes)
    {
        if (cur->u.object.header.color == Malang_Object::white)
        {
            free_object(reinterpret_cast<Malang_Object*>(&cur->u));
            ++freed;
        }
        else
        {
            cur->u.object.header.color = Malang_Object::white;
        }
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
    obj.fields = nullptr;
    // Pause the GC while values are being initialized so the GC doesn't free while
    // constructing.
    auto paused = m_is_paused;
    m_is_paused = true;
    // Init here:
    m_is_paused = paused;
}

void Malang_GC::construct_array(Malang_Array &arr, Type_Info *type, Fixnum length)
{
    assert(type);
    arr.header.type = type;
    arr.header.allocator = this;
    arr.header.free = false;
    arr.header.is_array = true;
    // don't want to immediately free this array...
    //arr.header.color = Malang_Object::grey;
    arr.header.color = Malang_Object::white;
    arr.size = length;
    if (length)
    {
        // @FixMe: should initialization be handled? maybe call ctor for every element
        arr.data = new Malang_Value[length];
        // Pause the GC while values are being initialized so the GC doesn't free while
        // constructing.
        auto paused = m_is_paused;
        m_is_paused = true;
        // Init here:
        m_is_paused = paused;
    }
    else
    {
        arr.data = nullptr;
    }
}

GC_Node *Malang_GC::alloc_intern()
{
    if (!m_is_paused && m_allocated.nodes.size() >= m_next_run)
    {
        m_next_run += m_run_interval;
        m_next_run = std::min(m_next_run, m_max_objects);
        printf("GC: automatic run triggered\n");
        mark_and_sweep();
    }
    if (m_allocated.nodes.size() >= m_max_objects)
    {
        panic("GC: out of alotted memory.\n");
    }
    auto gc_node = m_free.pop();
    if (!gc_node)
    {
        gc_node = new GC_Node{&m_allocated};
    }
    m_total_allocated++;
    return gc_node;
}

Malang_Object *Malang_GC::allocate_object(Type_Token type_token)
{
    auto gc_node = alloc_intern();
    auto type = m_types->get_type(type_token);
    construct_object(gc_node->u.object, type);
    m_allocated.append(gc_node);
    return &(gc_node->u.object.header);
}

Malang_Object *Malang_GC::allocate_array(Type_Token of_type_token, Fixnum length)
{
    auto gc_node = alloc_intern();
    auto type = m_types->get_type(of_type_token);
    construct_array(gc_node->u.array, type, length);
    m_allocated.append(gc_node);
    return &(gc_node->u.array.header);
}

void Malang_GC::free_object_body(Malang_Object_Body *obj)
{
    if (obj->header.free)
    {
        panic("GC: free_object_body attempted to double free");
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
        panic("GC: free_array attempted to double free");
    }
    if (arr->data)
    {
        delete[] arr->data;
        arr->data = nullptr;
    }
    arr->size = 0;
}

void Malang_GC::free_node(GC_Node *gc_node)
{
    m_allocated.remove(gc_node);
    m_free.append(gc_node);
}

void Malang_GC::free_object(Malang_Object *obj)
{
    assert(obj->allocator == this);
    assert(obj->free == false);
    if (obj->is_array)
    {
        free_array(reinterpret_cast<Malang_Array*>(obj));
    }
    else
    {
        free_object_body(reinterpret_cast<Malang_Object_Body*>(obj));
    }
    obj->free = true;
    auto ptr = reinterpret_cast<uintptr_t>(obj);
    ptr -= offsetof(GC_Node, u);
    ++m_total_freed;
    free_node(reinterpret_cast<GC_Node*>(ptr));
}

void Malang_GC::deallocate(Malang_Object *obj)
{
    free_object(obj);
}

void GC_List::append(GC_Node *node)
{
    assert(node);
    if (node->lookup_index != GC_Node::magic_index)
    {
        panic("GC append: tried to append with invalid index!\n");
    }
    node->magic = this;
    node->lookup_index = nodes.size();
    nodes.push_back(node);
}

void GC_List::remove(GC_Node *node)
{
    assert(node);
    if (node->magic != this)
    {
        panic("GC remove: magic number corrupted! %p %ld\n", node->magic, node->lookup_index);
    }
    if (nodes.empty())
    {
        panic("GC remove: nodes empty!\n");
    }
    nodes[node->lookup_index] = std::move(nodes.back());
    nodes[node->lookup_index]->lookup_index = node->lookup_index;
    nodes.pop_back();
    node->lookup_index = GC_Node::magic_index;
    node->magic = nullptr;
}

GC_Node *GC_List::pop()
{
    if (nodes.empty())
    {
        return nullptr;
    }
    auto node = nodes.back();
    nodes.pop_back();
    node->lookup_index = GC_Node::magic_index;
    node->magic = nullptr;
    return node;
}
