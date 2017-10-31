#include "string.hpp"
#include "primitive_helpers.hpp"
#include "../vm.hpp"
#include "../../type_map.hpp"
#include "object.hpp"


static Num_Fields_Limit length_idx;
static Num_Fields_Limit intern_data_idx;

static void string_index_get(Malang_VM &vm)
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

void Malang_Runtime::runtime_string_init(Bound_Function_Map &b, Type_Map &m)
{
    auto _string = m.get_string();
    auto _int    = m.get_int();

    length_idx = add_field(_string, "length", m.get_int(), true);
    intern_data_idx = add_field(_string, ".intern_data", m.get_void(), true);
    add_bin_op_method(b, m, _string, "[]", _int, _int, string_index_get);
}
