#ifndef MALANG_OBJECT_OBJ_STRING_HPP
#define MALANG_OBJECT_OBJ_STRING_HPP

#include "object.hpp"

struct Mal_String : Mal_Object
{
    std::string value;
    Mal_String(const std::string &value)
        : value(value)
        {}
    OBJECT_OVERRIDES;
};

#endif /* MALANG_OBJECT_OBJ_STRING_HPP */
