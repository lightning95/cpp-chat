#ifndef HTTPSOCKET_H
#define HTTPSOCKET_H

#include <QString>

#include "TcpSocket.h"
#include "HttpResponse.h"

class HttpSocket {
    TcpSocket &tcp_socket;

public:    

    HttpSocket(TcpSocket &);
    void write(const HttpResponse&);
};

#endif // HTTPSOCKET_H
