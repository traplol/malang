#include "array_buffer.hpp"
#include "primitive_helpers.hpp"
#include "../vm.hpp"
#include "../../type_map.hpp"

void Malang_Runtime::runtime_buffer_init(Bound_Function_Map&, Type_Map &m)
{
    auto length_field = new Field_Info{"length", m.get_int()};
    auto _buffer = m.get_buffer();
    assert(_buffer->add_field(length_field));
}
