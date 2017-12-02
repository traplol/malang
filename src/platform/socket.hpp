#ifndef MALANG_SOCKET_HPP
#define MALANG_SOCKET_HPP

#include <string>
#include <vector>
namespace plat
{
    enum socket_result
    {
        ERR_Close_Fail = -6,
        ERR_Recv_Fail = -5,
        ERR_Send_Fail = -4,
        ERR_Connection_Failed = -3,
        ERR_Socket_Acquisition_Failed = -2,
        ERR_Address_Info = -1,
        OK = 0,
    };

    using socket = void*;

    socket_result socket_open(const char *host, const char *port, socket &out_socket);
    socket_result socket_close(socket socket);
    socket_result socket_send(socket socket, const char *data, size_t size, size_t &bytes_written);
    socket_result socket_send(socket socket, const std::string &data, size_t &bytes_written);
    socket_result socket_send(socket socket, const std::vector<char> &data, size_t &bytes_written);
    socket_result socket_recv(socket socket, char *buffer, size_t buffer_len, size_t &bytes_read);
}

#endif /* MALANG_SOCKET_HPP */
