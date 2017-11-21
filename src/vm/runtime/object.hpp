#ifndef MALANG_VM_OBJECT_HPP
#define MALANG_VM_OBJECT_HPP

#include "reflection.hpp"
#include "primitive_types.hpp"

#define Object 1
#define Array 2
#define Buffer 3

struct Malang_Object
{
    Type_Info *type;
    struct Malang_GC *allocator;
    unsigned char free : 1;
    unsigned char color : 2;
    unsigned char object_tag : 5;
    static constexpr auto white = 0u;
    static constexpr auto grey  = 1u;
    static constexpr auto black = 2u;
    void gc_mark();
};


struct Malang_Object_Body
{
    Malang_Object header;
    // @TODO: think about virtual methods
    // for classes with virtual methods, field[0] could be an array of virtual methods,
    // the VM will need a "Call_Virtual_Method" instruction that takes an index into
    // this table and calls that
    Malang_Value *fields;

    void gc_mark();
};


struct Malang_Array
{
    Malang_Object header;
    Fixnum size;
    Malang_Value *data;

    void gc_mark();
};


struct Malang_Buffer
{
    Malang_Object header;
    Fixnum size;
    unsigned char *data;

    void gc_mark();
};

#endif /* MALANG_VM_OBJECT_HPP */
