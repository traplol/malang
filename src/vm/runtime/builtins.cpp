#include <string>
#include <sstream>
#include "builtins.hpp"
#include "../../type_map.hpp"
#include "../vm.hpp"
#include "../runtime.hpp"

#include <stdio.h>

static std::string to_string(Malang_Value &value)
{
    std::stringstream ss;
    if (value.is_object())
    {
        auto obj = value.as_object();
        // @TODO: Call the object's to_string method
        ss << "<" << obj->type->name() << "#" << obj << ">";
    }
    else if (value.is_fixnum())
    {
        ss << value.as_fixnum();
    }
    else if (value.is_double())
    {
        ss << value.as_double();
    }
    else if (value.is_pointer())
    {
        ss << value.as_pointer<void>();
    }
    else
    {
        ss << value.bits();
    }
    return ss.str();
}

static void println(Malang_VM &vm)
{
    auto top = vm.pop_data();
    printf("%s\n", to_string(top).c_str());
}

static void stack_trace(Malang_VM &vm)
{
    vm.stack_trace();
}

void Malang_Runtime::runtime_builtins_init(Primitive_Function_Map &b, Type_Map &t)
{
    Function_Type_Info *fn;
    const auto is_primitive = true;
    fn = t.declare_function({t.get_object()}, t.get_void(), is_primitive);
    b.add_builtin("println", fn, println);

    fn = t.declare_function({}, t.get_void(), is_primitive);
    b.add_builtin("stack_trace", fn, stack_trace);
}
