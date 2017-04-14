#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <QString>
#include <memory>

#include "FD.h"
#include "TcpServer.h"

class TcpServer;

class TcpSocket {
    static const int BUFFER_SIZE = 100000;
    TcpServer* server;
    size_t buffersize;
    char buffer[BUFFER_SIZE];
    FD fd;

public:
    TcpSocket(TcpServer*, FD &&);
    TcpSocket(TcpSocket &&);
    ~TcpSocket();
    int read(QString&);
    int write(const char * data, size_t size);
    void flush();

    TcpSocket &operator=(TcpSocket &&);

    friend class TcpServer;
};

#endif // TCPSOCKET_H
