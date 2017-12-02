#include "mod_socket.hpp"
#include "object.hpp"
#include "string.hpp"
#include "primitive_helpers.hpp"
#include "../../platform/socket.hpp"
#include "../../module_map.hpp"
#include "../../type_map.hpp"
#include "../../defer.hpp"
#include "../vm.hpp"
#include "../runtime.hpp"

static Type_Token socket_type_token;
static Num_Fields_Limit host_idx;
static Num_Fields_Limit port_idx;
static Num_Fields_Limit socket_idx;

inline static
Malang_Object_Body *cast(Malang_Object *obj)
{
    return reinterpret_cast<Malang_Object_Body*>(obj);
}
inline static
Malang_Object_Body *host(Malang_Object_Body *file)
{
    return cast(file->fields[host_idx].as_object());
}
inline static
Malang_Object_Body *port(Malang_Object_Body *file)
{
    return cast(file->fields[port_idx].as_object());
}
inline static
plat::socket socket(Malang_Object_Body *file)
{
    return reinterpret_cast<plat::socket>(file->fields[socket_idx].as_pointer());
}

inline static
void socket_construct(Malang_Object *place, Malang_Object *host, Malang_Object *port)
{
    assert(place);
    assert(host);
    assert(port);
    auto sock = cast(place);
    sock->fields[host_idx] = host;
    sock->fields[port_idx] = port;
    sock->fields[socket_idx] = (void*)nullptr;
}

// fn Socket.open() -> bool
static
void socket_open(Malang_VM &vm)
{
    auto sock_obj = cast(vm.pop_data().as_object());
    auto sock = socket(sock_obj);
    if (sock)
    {
        vm.push_data(true);
        return;
    }
    auto host_str = Malang_Runtime::string_alloc_c_str(host(sock_obj));
    assert(host_str);
    auto port_str = Malang_Runtime::string_alloc_c_str(port(sock_obj));
    assert(port_str);

    plat::socket new_sock;
    if (plat::socket_result::OK != plat::socket_open(host_str, port_str, new_sock))
    {
        vm.push_data(false);
        return;
    }
    sock_obj->fields[socket_idx] = new_sock;
    vm.push_data(true);
}

// fn Socket.close() -> void
static
void socket_close(Malang_VM &vm)
{
    auto sock_obj = cast(vm.pop_data().as_object());
    auto sock = socket(sock_obj);
    if (sock)
    {
        plat::socket_close(sock);
    }
}

// fn Socket.read(inbuf: buffer) -> int
static
void socket_recv(Malang_VM &vm)
{
    auto buf = reinterpret_cast<Malang_Buffer*>(vm.pop_data().as_object());
    auto sock_obj = cast(vm.pop_data().as_object());
    auto sock = socket(sock_obj);
    if (sock)
    {
        size_t bytes_read = 0;
        if (plat::socket_result::OK !=
            plat::socket_recv(sock, reinterpret_cast<char*>(buf->data), buf->size, bytes_read))
        {
            goto error;
        }
        vm.push_data(static_cast<Fixnum>(bytes_read));
        return;
    }
error:
    vm.push_data(0);
}

// fn Socket.write(outbuf: buffer) -> int
static
void socket_send_buffer(Malang_VM &vm)
{
    auto buf = reinterpret_cast<Malang_Buffer*>(vm.pop_data().as_object());
    auto sock_obj = cast(vm.pop_data().as_object());
    auto sock = socket(sock_obj);
    if (sock)
    {
        size_t bytes_written = 0;
        if (plat::socket_result::OK !=
            plat::socket_send(sock, reinterpret_cast<char*>(buf->data), buf->size, bytes_written))
        {
            goto error;
        }
        vm.push_data(static_cast<Fixnum>(bytes_written));
        return;
    }
error:
    vm.push_data(0);
}

// fn Socket.write(outbuf: string) -> int
static
void socket_send_string(Malang_VM &vm)
{
    auto str = cast(vm.pop_data().as_object());
    auto sock_obj = cast(vm.pop_data().as_object());
    auto sock = socket(sock_obj);
    if (sock)
    {
        auto len = Malang_Runtime::string_length(str);
        auto data = Malang_Runtime::string_data(str);
        size_t bytes_written = 0;
        if (plat::socket_result::OK !=
            plat::socket_send(sock, data, len, bytes_written))
        {
            goto error;
        }
        vm.push_data(static_cast<Fixnum>(bytes_written));
        return;
    }
error:
    vm.push_data(0);
}

// new(host: string, port: string)
static
void socket_string_string_new(Malang_VM &vm)
{
    auto port_str = vm.pop_data().as_object();
    auto host_str = vm.pop_data().as_object();
    auto socket_obj = vm.pop_data().as_object();
    socket_construct(socket_obj, host_str, port_str);
}

void Malang_Runtime::runtime_mod_socket_init(Bound_Function_Map &b, Type_Map &types, Module_Map &modules)
{
    auto old_mod = types.module();
    defer1(types.module(old_mod));

    auto socket_mod = modules.get({"std", "socket"}); // std::socket
    assert(socket_mod);
    types.module(socket_mod);
    // stop imports from attempting to load the module from disk
    socket_mod->builtin(true);

    auto _socket = types.declare_builtin_type("Socket", nullptr, true);
    auto _string = types.get_string();
    auto _void   = types.get_void();
    auto _int    = types.get_int();
    auto _bool   = types.get_bool();
    auto _buffer = types.get_buffer();

    assert(_socket);
    socket_type_token = _socket->type_token();
    host_idx = add_field(_socket, "host", _string, true, false);
    port_idx = add_field(_socket, "port", _string, true, false);
    socket_idx = add_field(_socket, ".socket", _void, true, true);

    add_constructor(b, types, _socket, {_string, _string}, socket_string_string_new);

    add_method(b, types, _socket, "open",  {}, _bool, socket_open);
    add_method(b, types, _socket, "close", {}, _void, socket_close);
    add_method(b, types, _socket, "recv",  {_buffer}, _int, socket_recv);
    add_method(b, types, _socket, "send", {_buffer}, _int, socket_send_buffer);
    add_method(b, types, _socket, "send", {_string}, _int, socket_send_string);
}
