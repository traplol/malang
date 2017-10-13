#include <cassert>
#include "object.hpp"

void Malang_Object::gc_mark()
{
    assert(free == false);

    if (color == black)
    {
        return;
    }

    color = grey;
    for (auto &&value : fields)
    {
        if (value.is_pointer())
        {
            value.as_pointer()->gc_mark();
        }
    }
    color = black;
}
