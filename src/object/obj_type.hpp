#ifndef MALANG_OBJECT_OBJ_TYPE_HPP
#define MALANG_OBJECT_OBJ_TYPE_HPP

#include "object.hpp"

struct Mal_Type : Mal_Object
{
    Type *value;
    Mal_Type(Type *value)
        : value(value)
        {}
    OBJECT_OVERRIDES;
};

#endif /* MALANG_OBJECT_OBJ_TYPE_HPP */
