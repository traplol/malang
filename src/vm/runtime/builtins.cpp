#include <string>
#include <sstream>
#include <iostream>

#include "builtins.hpp"
#include "string.hpp"
#include "../../type_map.hpp"
#include "../vm.hpp"
#include "../runtime.hpp"

#include <stdio.h>

static
void print_int(Malang_VM &vm)
{
    auto top = vm.pop_data().as_fixnum();
    printf("%d", top);
}

static
void print_char(Malang_VM &vm)
{
    auto top = vm.pop_data().as_fixnum();
    printf("%c", (Char)top);
}

static
void print_buffer(Malang_VM &vm)
{
    auto top = vm.pop_data().as_object();
    assert(top->object_tag == Buffer);
    auto buffer = reinterpret_cast<Malang_Buffer*>(top);
    for (Fixnum i = 0; i < buffer->size; ++i)
    {
        printf("%2x ", buffer->data[i]);
    }
    //printf("%.*s", buffer->size, buffer->data);
}

static
void print_double(Malang_VM &vm)
{
    auto top = vm.pop_data().as_double();
    printf("%lf", top);
}

static
void print_bool(Malang_VM &vm)
{
    auto top = vm.pop_data().as_fixnum();
    if (top == 0)
        printf("false");
    else
        printf("true");
}

static
void print_string(Malang_VM &vm)
{
    static bool once = true;
    static uint16_t length_idx = 0, intern_data_idx = 0;
    if (once)
    {
        auto str_ty = vm.gc->types()->get_string();
        if (!str_ty->get_field_index("length", length_idx))
        {
            printf("no length field in strings ???\n");
            abort();
        }
        if (!str_ty->get_field_index(".intern_data", intern_data_idx))
        {
            printf("no .intern_data field in strings ???\n");
            abort();
        }
        once = false;
    }

    auto top = vm.pop_data().as_object();
    auto string = reinterpret_cast<Malang_Object_Body*>(top);
    auto len = string->fields[length_idx].as_fixnum();
    if (len)
    {
        auto data = string->fields[intern_data_idx].as_pointer();
        printf("%.*s", len, static_cast<char*>(data));
    }
}

static
void println_int(Malang_VM &vm)
{
    print_int(vm);
    printf("\n");
}

static
void println_char(Malang_VM &vm)
{
    print_char(vm);
    printf("\n");
}

static
void println_buffer(Malang_VM &vm)
{
    print_buffer(vm);
    printf("\n");
}

static
void println_double(Malang_VM &vm)
{
    print_double(vm);
    printf("\n");
}

static
void println_bool(Malang_VM &vm)
{
    print_bool(vm);
    printf("\n");
}

static
void println_string(Malang_VM &vm)
{
    print_string(vm);
    printf("\n");
}

static
void println_empty(Malang_VM &)
{
    printf("\n");
}


static
void stack_trace(Malang_VM &vm)
{
    vm.stack_trace();
}

static
void gc_pause(Malang_VM &vm)
{
    vm.gc->disable_automatic();
}
static
void gc_resume(Malang_VM &vm)
{
    vm.gc->enable_automatic();
}
static
void gc_run(Malang_VM &vm)
{
    vm.gc->manual_run();
}

static
void breakpoint(Malang_VM &vm)
{
    vm.breaking = true;
}

static
void cast_int_to_char(Malang_VM &vm)
{
    auto top = vm.pop_data().as_fixnum();
    vm.push_data(static_cast<Char>(top));
}

static
void cast_char_to_int(Malang_VM &) {/*NOOP*/}

static
void cast_double_to_int(Malang_VM &vm)
{
    auto top = vm.pop_data().as_double();
    vm.push_data(static_cast<Fixnum>(top));
}

static
void cast_int_to_double(Malang_VM &vm)
{
    auto top = vm.pop_data().as_fixnum();
    vm.push_data(static_cast<Double>(top));
}

static
void get_char(Malang_VM &vm)
{
    vm.push_data(static_cast<Fixnum>(getchar()));
}

static
void get_line(Malang_VM &vm)
{
    std::string line;
    std::getline(std::cin, line);
    Malang_Runtime::string_alloc_push(vm, line);
}

void make_builtin(Bound_Function_Map &b, Type_Map &t, const std::string &name, Native_Code native_code, const std::vector<Type_Info*> &param_types, Type_Info *return_type)
{
    Function_Type_Info *fn;
    const auto is_native = true;
    fn = t.declare_function(param_types, return_type, is_native);
    b.add(name, fn, native_code);
}


static
void mal_exit(Malang_VM &vm)
{
    auto code = vm.pop_data().as_fixnum();
    exit(code);
}

void Malang_Runtime::runtime_builtins_init(Bound_Function_Map &b, Type_Map &t)
{
    // fn get_char() -> char
    make_builtin(b, t, "get_char", get_char,   {},    t.get_char());
    // fn get_line() -> string
    make_builtin(b, t, "get_line", get_line,   {},    t.get_string());

    // fn char(int) -> char
    make_builtin(b, t, "char",   cast_int_to_char,   {t.get_int()},    t.get_char());
    // fn int(char) -> int
    make_builtin(b, t, "int",    cast_char_to_int,   {t.get_char()},   t.get_int());
    // fn int(double) -> int
    make_builtin(b, t, "int",    cast_double_to_int, {t.get_double()}, t.get_int());
    // fn double(int) -> double
    make_builtin(b, t, "double", cast_int_to_double, {t.get_int()},    t.get_double());

    // fn println() -> void
    make_builtin(b, t, "println", println_empty,  {},               t.get_void());
    // fn println(int) -> void
    make_builtin(b, t, "println", println_int,    {t.get_int()},    t.get_void());
    // fn println(char) -> void
    make_builtin(b, t, "println", println_char,   {t.get_char()},   t.get_void());
    // fn println(bool) -> void
    make_builtin(b, t, "println", println_bool,   {t.get_bool()},   t.get_void());
    // fn println(double) -> void
    make_builtin(b, t, "println", println_double, {t.get_double()}, t.get_void());
    // fn println(buffer) -> void
    make_builtin(b, t, "println", println_buffer, {t.get_buffer()}, t.get_void());
    // fn println(string) -> void
    make_builtin(b, t, "println", println_string, {t.get_string()}, t.get_void());

    // fn print(int) -> void
    make_builtin(b, t, "print", print_int,    {t.get_int()},    t.get_void());
    // fn print(char) -> void
    make_builtin(b, t, "print", print_char,   {t.get_char()},   t.get_void());
    // fn print(bool) -> void
    make_builtin(b, t, "print", print_bool,   {t.get_bool()},   t.get_void());
    // fn print(double) -> void
    make_builtin(b, t, "print", print_double, {t.get_double()}, t.get_void());
    // fn print(buffer) -> void
    make_builtin(b, t, "print", print_buffer, {t.get_buffer()}, t.get_void());
    // fn print(string) -> void
    make_builtin(b, t, "print", print_string, {t.get_string()}, t.get_void());

    // fn exit(int) -> void
    make_builtin(b, t, "exit",  mal_exit,  {t.get_int()}, t.get_void());

    // fn stack_trace() -> void
    make_builtin(b, t, "stack_trace", stack_trace, {}, t.get_void());
    // fn gc_pause() -> void
    make_builtin(b, t, "gc_pause",    gc_pause,    {}, t.get_void());
    // fn gc_resume() -> void
    make_builtin(b, t, "gc_resume",   gc_resume,   {}, t.get_void());
    // fn gc_run() -> void
    make_builtin(b, t, "gc_run",      gc_run,      {}, t.get_void());
    // fn breakpoint() -> void
    make_builtin(b, t, "breakpoint",  breakpoint,  {}, t.get_void());
}
