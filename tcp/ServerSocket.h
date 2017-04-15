#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H
#include "tcp/Socket.h"

class ServerSocket : public Socket {
private:
    int port;

public:
    ServerSocket(int port, std::function<void(uint32_t, ServerSocket &)> const&);
    ServerSocket(ServerSocket &&);
    void handle(uint32_t event);

    static void listen(FD const&);
    static FD createAndBind(int);
};

#endif // SERVERSOCKET_H

