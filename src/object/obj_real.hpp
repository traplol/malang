#ifndef MALANG_OBJECT_OBJ_REAL_HPP
#define MALANG_OBJECT_OBJ_REAL_HPP

#include "object.hpp"

struct Mal_Real : Mal_Object
{
    double value;
    Mal_Real(double value)
        : value(value)
        {}
    OBJECT_OVERRIDES;
};

#endif /* MALANG_OBJECT_OBJ_REAL_HPP */
