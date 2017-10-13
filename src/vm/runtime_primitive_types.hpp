#ifndef MALANG_RUNTIME_PRIMITIVE_TYPES_HPP
#define MALANG_RUNTIME_PRIMITIVE_TYPES_HPP

#include <stdint.h>
#include "nun_boxing.hpp"

using Fixnum = int32_t;
using Double = double;
using Char = char;

template<typename T>
struct Array
{
    Fixnum length;
    const T *data;
};

using String = Array<Char>;
using Malang_Value = Value<struct Malang_Object>;


#endif
