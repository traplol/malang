#include "mod_file.hpp"
#include "object.hpp"
#include "string.hpp"
#include "primitive_helpers.hpp"
#include "../../module_map.hpp"
#include "../../type_map.hpp"
#include "../../defer.hpp"
#include "../vm.hpp"
#include "../runtime.hpp"

static Type_Token file_type_token;
static Num_Fields_Limit path_idx;
static Num_Fields_Limit file_desc_idx;

// @TODO: Move all of these File operations into platform.hpp and use move semantics for
// the read operations.

inline static
Malang_Object_Body *cast(Malang_Object *obj)
{
    return reinterpret_cast<Malang_Object_Body*>(obj);
}
inline static
Malang_Object_Body *path(Malang_Object_Body *file)
{
    return cast(file->fields[path_idx].as_object());
}
inline static
FILE *file_desc(Malang_Object_Body *file)
{
    return reinterpret_cast<FILE*>(file->fields[file_desc_idx].as_pointer());
}
inline static
void file_construct(Malang_Object *place, Malang_Object *path)
{
    assert(place);
    assert(path);
    auto file = cast(place);
    file->fields[path_idx] = path;
    file->fields[file_desc_idx] = (void*)nullptr;
}

// fn File.open(access_flags: string) -> bool
// returns true on success, false otherwise
static
void file_open(Malang_VM &vm)
{
    auto flags = cast(vm.pop_data().as_object());
    auto file = cast(vm.pop_data().as_object());
    auto fp = file_desc(file);
    if (fp)
    {
        fclose(fp);
    }
    auto p = path(file);
    auto path_str = Malang_Runtime::string_alloc_c_str(p);
    assert(path_str);
    auto flags_str = Malang_Runtime::string_alloc_c_str(flags);
    assert(flags_str);
    auto new_fp = fopen(path_str, flags_str);
    file->fields[file_desc_idx] = new_fp;
    vm.push_data(new_fp != nullptr); // return value
    delete[] path_str;
    delete[] flags_str;
}

// fn File.close() -> void
static
void file_close(Malang_VM &vm)
{
    auto file = cast(vm.pop_data().as_object());
    auto fp = file_desc(file);
    if (fp)
    {
        fclose(fp);
    }
    file->fields[file_desc_idx] = (void*)nullptr;
}

// fn File.read(out: buffer) -> int
// fills the given buffer and returns the number of bytes read;
// this may be less than the size of the buffer provided.
static
void file_read(Malang_VM &vm)
{
    auto buf = reinterpret_cast<Malang_Buffer*>(vm.pop_data().as_object());
    auto file = cast(vm.pop_data().as_object());
    auto fp = file_desc(file);
    Fixnum ret = 0;
    if (fp)
    {
        ret = fread(buf->data, 1, buf->size, fp);
    }
    else // file not open
    {
        auto p = path(file);
        auto p_str = Malang_Runtime::string_alloc_c_str(p);
        assert(p_str);
        fp = fopen(p_str, "rb");
        if (fp)
        {
            ret = fread(buf->data, 1, buf->size, fp);
            fclose(fp);
        }
        delete[] p_str;
    }
    vm.push_data(ret);
}

// fn File.write(data: buffer) -> int
// writes the contents of the buffer to a file and returns the number of bytes written
static
void file_write_buffer(Malang_VM &vm)
{
    auto buf = reinterpret_cast<Malang_Buffer*>(vm.pop_data().as_object());
    auto file = cast(vm.pop_data().as_object());
    auto fp = file_desc(file);
    Fixnum ret = 0;
    if (fp)
    {
        ret = fwrite(buf->data, 1, buf->size, fp);
    }
    else // file not open
    {
        auto p = path(file);
        auto p_str = Malang_Runtime::string_alloc_c_str(p);
        assert(p_str);
        fp = fopen(p_str, "wb");
        if (fp)
        {
            ret = fwrite(buf->data, 1, buf->size, fp);
            fclose(fp);
        }
        delete[] p_str;
    }
    vm.push_data(ret);
}

// fn File.write(data: string) -> int
// writes the contents of the string to a file and returns the number of bytes written
static
void file_write_string(Malang_VM &vm)
{
    auto str = cast(vm.pop_data().as_object());
    auto file = cast(vm.pop_data().as_object());
    auto fp = file_desc(file);
    Fixnum ret = 0;
    if (fp)
    {
        auto len = Malang_Runtime::string_length(str);
        auto data = Malang_Runtime::string_data(str);
        ret = fwrite(data, 1, len, fp);
    }
    else // file not open
    {
        auto p = path(file);
        auto p_str = Malang_Runtime::string_alloc_c_str(p);
        assert(p_str);
        fp = fopen(p_str, "wb");
        if (fp)
        {
            auto len = Malang_Runtime::string_length(str);
            auto data = Malang_Runtime::string_data(str);
            ret = fwrite(data, 1, len, fp);
            fclose(fp);
        }
        delete[] p_str;
    }
    vm.push_data(ret);
}

// fn File.read_all() -> buffer
// returns a buffer filled with the contents of the file
static
void file_read_all(Malang_VM &vm)
{
    auto file = cast(vm.pop_data().as_object());
    auto fp = file_desc(file);
    if (fp)
    {
        fseek(fp, 0, SEEK_END);   // non-portable
        auto size = ftell(fp);
        rewind(fp);
        auto buf = reinterpret_cast<Malang_Buffer*>(vm.gc->allocate_buffer(size));
        assert(buf);
        fread(buf->data, 1, size, fp);
        vm.push_data(reinterpret_cast<Malang_Object*>(buf));
    }
    else // file not open
    {
        auto p = path(file);
        auto p_str = Malang_Runtime::string_alloc_c_str(p);
        assert(p_str);
        fp = fopen(p_str, "rb");
        if (fp)
        {
            fseek(fp, 0, SEEK_END);   // non-portable
            auto size = ftell(fp);
            rewind(fp);
            auto buf = reinterpret_cast<Malang_Buffer*>(vm.gc->allocate_buffer(size));
            assert(buf);
            fread(buf->data, 1, size, fp);
            vm.push_data(reinterpret_cast<Malang_Object*>(buf));
            fclose(fp);
        }
        else
        {
            auto empty_buf = vm.gc->allocate_buffer(0);
            vm.push_data(reinterpret_cast<Malang_Object*>(empty_buf));
        }
        delete[] p_str;
    }
}

// File(path: string)
// creates a new File instance but does not read the file.
static
void file_string_new(Malang_VM &vm)
{
    auto path_str = vm.pop_data().as_object();
    auto file = vm.pop_data().as_object();
    file_construct(file, path_str);
}

void Malang_Runtime::runtime_mod_file_init(Bound_Function_Map &b, Type_Map &types, Module_Map &modules)
{
    auto old_mod = types.module();
    defer1(types.module(old_mod));

    auto file_mod = modules.get({"std", "file"}); // std$file
    assert(file_mod);
    types.module(file_mod);
    // stop imports from attempting to load the module from disk
    file_mod->builtin(true);

    auto _file   = types.declare_builtin_type("File", nullptr, true);
    auto _string = types.get_string();
    auto _void   = types.get_void();
    auto _int    = types.get_int();
    auto _bool   = types.get_bool();
    auto _buffer = types.get_buffer();

    assert(_file);
    file_type_token = _file->type_token();
    path_idx = add_field(_file, "path", _string, true, false);
    file_desc_idx = add_field(_file, ".file_desc", _void, true, true);

    add_constructor(b, types, _file, {_string}, file_string_new);

    add_method(b, types, _file, "open",     {_string}, _bool, file_open);
    add_method(b, types, _file, "close",    {}, _void, file_close);
    add_method(b, types, _file, "read",     {_buffer}, _int, file_read);
    add_method(b, types, _file, "read_all", {}, _buffer, file_read_all);
    add_method(b, types, _file, "write",    {_buffer}, _int, file_write_buffer);
    add_method(b, types, _file, "write",    {_string}, _int, file_write_string);

    //_file->dump();
}
