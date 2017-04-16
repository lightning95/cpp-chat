#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <exception>

#include "library/EpollWrap.h"
#include "library/FD.h"
#include "tcp/TcpSocket.h"
#include "tcp/ServerSocket.h"

class TcpSocket;

enum EventType {NEWDATA, ERROR, HUP};

class TcpServer {
    std::map<int, Socket&>      callbacks;
    std::map<int, TcpSocket>    tcpSockets;
    std::map<int, ServerSocket> serverSockets;

    EpollWrap &epoll;
    volatile bool running;

    void connectionHandler(std::function<void(TcpSocket &, EventType)>, ServerSocket &, uint32_t);
    void dataHandler(std::function<void(TcpSocket &, EventType)>, TcpSocket &, uint32_t);

public:
    TcpServer();
    ~TcpServer();

    void addPort(int, std::function<void(TcpSocket &, EventType)>); //port, new data callback
    void start();
    void stop();
friend class TcpSocket;
};


#endif // TCPSERVER_H
