#include "string.hpp"
#include "primitive_helpers.hpp"
#include "../vm.hpp"
#include "../../type_map.hpp"
#include "object.hpp"
#include "gc.hpp"

using namespace Malang_Runtime;

static Type_Token string_type_token;
static Num_Fields_Limit length_idx;
static Num_Fields_Limit intern_data_idx;

inline static
Malang_Object_Body *cast(Malang_Object *obj)
{
    return reinterpret_cast<Malang_Object_Body*>(obj);
}
inline static
Fixnum length(Malang_Object_Body *str)
{
    return str->fields[length_idx].as_fixnum();
}
inline static
Char *data(Malang_Object_Body *str)
{
    return reinterpret_cast<Char*>(str->fields[intern_data_idx].as_pointer());
}

void Malang_Runtime::string_construct_intern(Malang_Object *place, Fixnum size, char *buffer)
{
    auto str = cast(place);
    str->fields[length_idx] = size;
    str->fields[intern_data_idx] = buffer;
}

void Malang_Runtime::string_construct_intern(Malang_Object *place, const String_Constant &string_constant)
{
    string_construct_intern(place, string_constant.size(), string_constant.data_copy());
}

// string(buf: buffer)
static
void string_buffer_new(Malang_VM &vm)
{
    auto arg_1 = vm.pop_data().as_object();
    assert(arg_1->object_tag == Buffer);
    auto buffer = reinterpret_cast<Malang_Buffer*>(arg_1);
    auto arg_0 = vm.pop_data().as_object();
    auto str_ref = reinterpret_cast<Malang_Object_Body*>(arg_0);
    assert(str_ref->header.type->type_token() == string_type_token);

    Fixnum size = 0;
    auto copy = new Char[size];
    for (size = 0; size < buffer->size; ++size)
    {
        if (buffer->data[size] == 0)
        {
            break;
        }
        copy[size] = buffer->data[size];
    }
    string_construct_intern(arg_0, size, copy);
}

// string(buf: buffer, num_chars: int)
static
void string_buffer_int_new(Malang_VM &vm)
{
    auto num_chars = vm.pop_data().as_fixnum();
    auto arg_1 = vm.pop_data().as_object();
    assert(arg_1->object_tag == Buffer);
    auto buffer = reinterpret_cast<Malang_Buffer*>(arg_1);
    auto arg_0 = vm.pop_data().as_object();
    auto str_ref = reinterpret_cast<Malang_Object_Body*>(arg_0);
    assert(str_ref->header.type->type_token() == string_type_token);

    Fixnum size = 0;
    auto copy = new Char[size];
    for (size = 0; size < num_chars && buffer->size; ++size)
    {
        if (buffer->data[size] == 0)
        {
            break;
        }
        copy[size] = buffer->data[size];
    }
    string_construct_intern(arg_0, size, copy);
}

static
void string_index_get(Malang_VM &vm)
{
    auto idx = vm.pop_data().as_fixnum();
    auto str = reinterpret_cast<Malang_Object_Body*>(vm.pop_data().as_object());
    auto str_buf = static_cast<char*>(str->fields[intern_data_idx].as_pointer());
    auto str_len = str->fields[length_idx].as_fixnum();
    if (idx < 0 || idx >= str_len)
    {
        vm.panic("Attempted to access string index with %d but the string's length was %d!",
                 idx, str_len);
    }
    vm.push_data(str_buf[idx]);
}

static
void string_string_add(Malang_VM &vm)
{
    auto b = cast(vm.pop_data().as_object());
    auto a = cast(vm.pop_data().as_object());
    
    auto c = vm.gc->allocate_unmanaged_object(string_type_token);
    auto buff = new Char[length(a) + length(b)];
    Fixnum n = 0;
    for (Fixnum i = 0; i < length(a); ++i, ++n)
    {
        buff[n] = data(a)[i];
    }
    for (Fixnum i = 0; i < length(b); ++i, ++n)
    {
        buff[n] = data(b)[i];
    }
    string_construct_intern(c, length(a) + length(b), buff);
    vm.push_data(c);
    vm.gc->manage(c);
}

void Malang_Runtime::runtime_string_init(Bound_Function_Map &b, Type_Map &m)
{
    auto _string = m.get_string();
    auto _int    = m.get_int();
    auto _buffer = m.get_buffer();

    string_type_token = _string->type_token();

    length_idx = add_field(_string, "length", _int, true);
    intern_data_idx = add_field(_string, ".intern_data", m.get_void(), true);

    add_constructor(b, m, _string, {_buffer}, string_buffer_new);
    add_constructor(b, m, _string, {_buffer, _int}, string_buffer_int_new);

    add_bin_op_method(b, m, _string, "[]", _int, _int, string_index_get);
    add_bin_op_method(b, m, _string, "+", _string, _string, string_string_add);
}
