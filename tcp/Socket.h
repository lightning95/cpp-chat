#ifndef SOCKET_H
#define SOCKET_H

#include "library/EpollWrap.h"

class Socket {
protected:
    FD fd;
    std::function<void(uint32_t, Socket &)> handler;
public:
    Socket(FD &&, std::function<void(uint32_t, Socket &)> const &);
    Socket(Socket &&);

    int getfd() const;

    static const int QUEUE_SIZE = 10;
    static void epollAdd(FD const&);

    virtual ~Socket();
    virtual void handle(uint32_t) = 0;

};

#endif // SOCKET_H
