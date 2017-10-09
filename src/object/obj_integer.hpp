#ifndef MALANG_OBJECT_OBJ_INTEGER_HPP
#define MALANG_OBJECT_OBJ_INTEGER_HPP

#include "object.hpp"

struct Mal_Integer : Mal_Object
{
    int64_t value;
    Mal_Integer(int64_t value)
        : value(value)
        {}
    OBJECT_OVERRIDES;

    static void register_type();
};

#endif /* MALANG_OBJECT_OBJ_INTEGER_HPP */
