#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTextStream>
#include <QString>

#include "http/HttpResponse.h"
#include "http/HttpSocket.h"
#include "http/HttpRequest.h"
#include "tcp/TcpServer.h"

class HttpServer {
//    QMap<TcpSocket*, HttpRequest> requests;
    TcpServer tcpServer;
    void newRequest(std::function<void(HttpRequest&, HttpSocket&)>, TcpSocket &, EventType);

public:
    HttpServer();
    void addPort(int, std::function<void(HttpRequest&, HttpSocket&)>);
    void start();
    void stop();
};


#endif // HTTPSERVER_H
