#include "tcp/Socket.h"
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

Socket::Socket(FD &&fd, std::function<void(uint32_t, Socket &)> const &handler) : fd(std::move(fd)), handler(handler) {
}

Socket::Socket(Socket &&sock) : fd(std::move(sock.fd)), handler(std::move(sock.handler)) {
}

Socket::~Socket() {
    if (fd.getfd() != FD::NONE) {
        EpollWrap::getInstance().remove(fd.getfd());
    }
}

int Socket::getfd() const {
    return fd.getfd();
}

void Socket::makeSocketNonBlocking(FD const& fd) {
    int flags = fcntl (fd.getfd(), F_GETFL, 0);
    if (flags == -1) //return -1;
        throw std::runtime_error("Can't make socket nonblocking");
    flags |= O_NONBLOCK;
    if (fcntl (fd.getfd(), F_SETFL, flags) == -1) //return -1;
        throw std::runtime_error("Can't make socket nonblocking");
}

void Socket::epollAdd(FD const& fd){
    if (EpollWrap::getInstance().add(fd.getfd(), EPOLLIN | EPOLLET | EPOLLRDHUP) < 0)
        throw std::runtime_error("Can't add fd to epoll");
}
