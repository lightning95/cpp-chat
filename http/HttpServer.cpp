#include "HttpServer.h"

HttpServer::HttpServer() {
}

void HttpServer::addPort(int port, std::function<void(HttpRequest&, HttpSocket&)> queryHandler) {
    tcpServer.addPort(port, bind(&HttpServer::newRequest, this, queryHandler, std::placeholders::_1, std::placeholders::_2));
}

void HttpServer::start() {
    tcpServer.start();
}

void HttpServer::stop() {
    tcpServer.stop();
}

void HttpServer::newRequest(std::function<void(HttpRequest&, HttpSocket&)> callBack,
                TcpSocket &socket, EventType type) {
    if (type == ERROR || type == HUP) {
        return;
    }
    if (type == NEWDATA) {
        QString str;
        int res = socket.read(str);  // 0 - read returned 0, -1 - error, 1 - EAGAIN
        if (res < 0) {
            return;
        }
        HttpRequest req;
        req.append(str);
        if (req.isValid()) {
            std::cout << "new Request" << std::endl;
            HttpSocket http_s(socket);
            callBack(req, http_s);
        }
    }
}
