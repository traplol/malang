#ifndef MALANG_RUNTIME_PRIMITIVE_TYPES_HPP
#define MALANG_RUNTIME_PRIMITIVE_TYPES_HPP

#include <stdint.h>
#include "../nun_boxing.hpp"

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
using Native_Code = void(*)(struct Malang_VM &vm);

struct Primitive_Function
{
    Primitive_Function(Fixnum index, Native_Code native_code, struct Function_Type_Info *fn_type)
        : index(index)
        , native_code(native_code)
        , fn_type(fn_type)
        {}
    Fixnum index;
    Native_Code native_code;
    struct Function_Type_Info *fn_type;
};


#endif
