#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>

#include "socket.hpp"

static inline
int to_sockfd(plat::socket sock)
{
    return static_cast<int>(reinterpret_cast<uintptr_t>(sock));
}
static inline
plat::socket from_sockfd(int sockfd)
{
    return reinterpret_cast<plat::socket>(static_cast<uintptr_t>(sockfd));
}

plat::socket_result plat::socket_close(socket socket)
{
    if (::close(to_sockfd(socket)) < 0)
    {
        return ERR_Close_Fail;
    }
    return OK;
}

plat::socket_result plat::socket_open(const char *host, const char *port, socket &out_socket)
{
    addrinfo hints{};
    addrinfo *res = nullptr;
    socket_result err = OK;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rc;
    if ((rc = getaddrinfo(host, port, &hints, &res)) < 0)
    {
        return ERR_Address_Info;
    }

    auto s = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s < 0)
    {
        err = ERR_Socket_Acquisition_Failed;
        goto error;
    }

    if (::connect(s, res->ai_addr, res->ai_addrlen) < 0)
    {
        err = ERR_Connection_Failed;
        goto error;
    }

    out_socket = from_sockfd(s);

error:
    freeaddrinfo(res);
    return err;
}

plat::socket_result plat::socket_send(socket socket, const char *data, size_t size, size_t &written)
{
    auto sfd = to_sockfd(socket);
    written = 0;
    int rc = 0;
    while (written < size)
    {
        rc = send(sfd, data + written, size - written, 0);
        if (rc < 0)
        {
            return ERR_Send_Fail;
        }
        written += rc;
    }
    return OK;
}

plat::socket_result plat::socket_send(socket socket, const std::string &data, size_t &written)
{
    return socket_send(socket, data.data(), data.size(), written);
}

plat::socket_result plat::socket_send(socket socket, const std::vector<char> &data, size_t &written)
{
    return socket_send(socket, data.data(), data.size(), written);
}

plat::socket_result plat::socket_recv(socket socket, char *buffer, size_t buffer_len, size_t &read)
{
    auto sfd = to_sockfd(socket);
    auto rc = recv(sfd, buffer, buffer_len, 0);
    if (rc < 0)
    {
        return ERR_Recv_Fail;
    }
    read = rc;
    return OK;
}

