#include "tcp/ServerSocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <exception>
#include <memory.h>
#include <iostream>

using std::cout;

ServerSocket::ServerSocket(int port, std::function<void (uint32_t, ServerSocket &)> const &handler)
    : Socket(createAndBind(port), [handler](uint32_t a, Socket &socket) {
        return handler(a, dynamic_cast<ServerSocket &>(socket));
    })
    , port(port) {
    makeSocketNonBlocking(fd);
    epollAdd(fd);
}

ServerSocket::ServerSocket(ServerSocket &&socket)
    : Socket(std::move(socket))
    , port(socket.port) {
}

void ServerSocket::handle(uint32_t event) {
    handler(event, *this);
}

FD ServerSocket::createAndBind(int port) {
    addrinfo hints;
    addrinfo *result, *rp;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string portstr = std::to_string(port);
    if (getaddrinfo (NULL, portstr.c_str(), &hints, &result) != 0)
        throw std::runtime_error("Can't getAddrInfo " + std::to_string(port));

    class ai {
        addrinfo *a;
    public :
        ai(addrinfo *a) : a(a) {}
        ~ai() {
            freeaddrinfo(a);
        }
    } tmp(result);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        FD res(socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol));
        if (res.getfd() == -1) continue;
        int t = 1;
        if (setsockopt(res.getfd(), SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)) == -1) {
            cout << "Can't make socket reusable\n";
            continue;
        }
        if (bind(res.getfd(), rp->ai_addr, rp->ai_addrlen) == 0){
            if (::listen(res.getfd(), QUEUE_SIZE) == -1)
                throw std::runtime_error("Listen error");
            return res;
        }
    }
    throw std::runtime_error("No address succeeded");
}

void ServerSocket::makeSocketNonBlocking(FD const& fd) {
    int flags = fcntl (fd.getfd(), F_GETFL, 0);
    if (flags == -1) //return -1;
        throw std::runtime_error("Can't make socket nonblocking");
    flags |= O_NONBLOCK;
    if (fcntl (fd.getfd(), F_SETFL, flags) == -1) //return -1;
        throw std::runtime_error("Can't make socket nonblocking");
}
