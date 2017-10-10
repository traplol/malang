#ifndef MALANG_RUNTIME_PRIMITIVE_TYPES_HPP
#define MALANG_RUNTIME_PRIMITIVE_TYPES_HPP

#include <stdint.h>

using Integer = int64_t;
using Double = double;
using Single = float;
using Char = char;

template<typename T>
struct Array
{
    Integer length;
    const T *data;
};

using String = Array<Char>;

#endif
