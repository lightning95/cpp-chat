#ifndef HTTPSOCKET_H
#define HTTPSOCKET_H

#include <QString>

#include "tcp/TcpSocket.h"
#include "http/HttpResponse.h"

class HttpSocket {
    TcpSocket &tcp_socket;

public:    

    HttpSocket(TcpSocket &);
    void write(const HttpResponse&);
};

#endif // HTTPSOCKET_H
