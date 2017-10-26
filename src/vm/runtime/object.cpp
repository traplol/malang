#include <cassert>
#include "object.hpp"

void Malang_Object::gc_mark()
{
    assert(free == false);
    assert(type);
    assert(allocator);

    // marking or already marked, this also serves as a cycle check
    if (color != white)
    {
        return;
    }

    color = grey;
    for (size_t i = 0; i < type->fields().size(); ++i)
    {
        auto value = fields[i];
        if (value.is_object())
        {
            value.as_object()->gc_mark();
        }
    }
    color = black;
}
