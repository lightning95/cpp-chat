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

#include "EpollWrap.h"
#include "FD.h"
#include "TcpSocket.h"

class TcpSocket;

enum EventType {NEWDATA, ERROR, HUP};

class TcpServer {
    static const int QUEUE_SIZE = 10;

    std::map<int, TcpSocket> sockets;
    std::map<int, std::function<void(uint32_t)>> callbacks;

    EpollWrap &epoll;
    std::vector<FD> tcpsockets;
    bool running;

    static void listenSocket(FD const&);
    static void makeSocketNonBlocking(FD const&);
    static FD createAndBind(int);//gets port, returns fd

    void connectionHandler(std::function<void(TcpSocket &, EventType)>, int, uint32_t);
    void dataHandler(std::function<void(TcpSocket &, EventType)>, int, uint32_t);

public:
    TcpServer();
    ~TcpServer();

    void addPort(int, std::function<void(TcpSocket &, EventType)>); //port, new data callback
    void start();
    void stop();
friend class TcpSocket;
};


#endif // TCPSERVER_H
