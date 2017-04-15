#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <QString>
#include <memory>

#include "library/FD.h"
#include "tcp/Socket.h"

class TcpServer;

class TcpSocket : public Socket {
    static const int BUFFER_SIZE = 100000;
    size_t buffersize;
    char buffer[BUFFER_SIZE];

public:
    TcpSocket(FD &&fd, std::function<void(uint32_t, TcpSocket &)> const &);
    TcpSocket(TcpSocket &&);
    ~TcpSocket();
    int read(QString&);
    int write(const char * data, size_t size);
    void flush();
    void handle(uint32_t event);

    TcpSocket &operator=(TcpSocket &&);
};

#endif // TCPSOCKET_H
