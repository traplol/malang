#ifndef MALANG_OBJECT_OBJ_BOOLEAN_HPP
#define MALANG_OBJECT_OBJ_BOOLEAN_HPP

#include "object.hpp"

struct Mal_Boolean : Mal_Object
{
    bool value;
    Mal_Boolean(bool value)
        : value(value)
        {}
    OBJECT_OVERRIDES;
};

#endif /* MALANG_OBJECT_OBJ_BOOLEAN_HPP */
