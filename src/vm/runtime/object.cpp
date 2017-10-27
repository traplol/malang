#include <cassert>
#include "object.hpp"

void Malang_Object::gc_mark()
{
    if (is_array)
    {
        reinterpret_cast<Malang_Array*>(this)->gc_mark();
    }
    else
    {
        reinterpret_cast<Malang_Object_Body*>(this)->gc_mark();
    }
}

void Malang_Object_Body::gc_mark()
{
    assert(header.free == false);
    assert(header.type);
    assert(header.allocator);

    // marking or already marked, this also serves as a cycle check
    if (header.color != header.white)
    {
        return;
    }

    header.color = header.grey;
    for (size_t i = 0; i < header.type->fields().size(); ++i)
    {
        auto value = fields[i];
        if (value.is_object())
        {
            value.as_object()->gc_mark();
        }
    }
    header.color = header.black;
}

void Malang_Array::gc_mark()
{
    assert(header.free == false);
    assert(header.type);
    assert(header.allocator);

    // marking or already marked, this also serves as a cycle check
    if (header.color != header.white)
    {
        return;
    }

    header.color = header.grey;
    for (size_t i = 0; i < static_cast<size_t>(size); ++i)
    {
        auto value = data[i];
        if (value.is_object())
        {
            value.as_object()->gc_mark();
        }
    }
    header.color = header.black;
}
