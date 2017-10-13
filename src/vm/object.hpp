#ifndef MALANG_VM_OBJECT_HPP
#define MALANG_VM_OBJECT_HPP

#include "reflection.hpp"
#include "runtime_primitive_types.hpp"

struct Malang_Object
{

    Type_Info *type;
    struct Malang_GC *allocator;
    unsigned char free : 1;
    unsigned char gc_allocated : 1;
    unsigned char color : 2;
    static constexpr auto white = 0u;
    static constexpr auto grey  = 1u;
    static constexpr auto black = 2u;

    // TODO: think about virtual methods
    // for classes with virtual methods, field[0] could be an array of virtual methods,
    // the VM will need a "Call_Virtual_Method" instruction that takes an index into
    // this table and calls that

    std::vector<Malang_Value> fields;

    void gc_mark();
};

#endif /* MALANG_VM_OBJECT_HPP */
