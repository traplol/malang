#include "array_buffer.hpp"
#include "primitive_helpers.hpp"
#include "../vm.hpp"
#include "../../type_map.hpp"

void Malang_Runtime::runtime_buffer_init(Bound_Function_Map&, Type_Map &m)
{
    auto _buffer = m.get_buffer();
    add_field(_buffer, "length", m.get_int(), true, false);
}
