#include "string.hpp"
#include "primitive_helpers.hpp"
#include "../vm.hpp"
#include "../../type_map.hpp"

static void s_ctor(Malang_VM &vm)
{
    auto string = vm.pop_data();
}

static void s_length(Malang_VM &vm)
{
    auto string = vm.pop_data();
}

void make_method(Primitive_Function_Map &f, Type_Map &t, Type_Info *for_type, const std::string &name, Native_Code native_code, const std::vector<Type_Info*> &param_types, Type_Info *return_type)
{
    auto fn_type = t.declare_function(param_types, return_type, true);
    auto pfn = f.add_primitive(fn_type, native_code);
    auto method_info = new Method_Info{name, fn_type, pfn};
    if (!for_type->add_method(method_info))
    {
        printf("couldn't add %s :: %s to %s\n",
               name.c_str(),
               fn_type->name().c_str(),
               for_type->name().c_str());
        delete method_info;
    }
}

void make_ctor(Primitive_Function_Map &f, Type_Map &t, Type_Info *for_type, Native_Code native_code, const std::vector<Type_Info*> &param_types)
{
    auto fn_type = t.declare_function(param_types, t.get_void(), true);
    auto pfn = f.add_primitive(fn_type, native_code);
    auto method_info = new Method_Info{".ctor", fn_type, pfn};
    if (!for_type->add_constructor(method_info))
    {
        printf("couldn't add .ctor :: %s to %s\n",
               fn_type->name().c_str(),
               for_type->name().c_str());
        delete method_info;
    }
}

void Malang_Runtime::runtime_string_init(Primitive_Function_Map &f, Type_Map &t)
{
    auto _string = t.get_string();
    make_ctor(f, t, _string, s_ctor, {_string});
    make_method(f, t, _string, "length", s_length, {}, t.get_int());
}

