#include "string.hpp"
#include "primitive_helpers.hpp"
#include "../vm.hpp"
#include "../../type_map.hpp"

void Malang_Runtime::runtime_string_init(Bound_Function_Map&, Type_Map &m)
{
    auto _string = m.get_string();
    auto length_field      = new Field_Info{"length", m.get_int()};
    _string->add_field(length_field);
    auto intern_data_field = new Field_Info{".intern_data", m.get_buffer()};
    _string->add_field(intern_data_field);
}
