#include "HttpSocket.h"

 HttpSocket::HttpSocket(TcpSocket &s): tcp_socket(s) {
}

void HttpSocket::write(const HttpResponse& response) {
    std::cout << "Writing in HTTP socket." << std::endl;
    tcp_socket.write(response.header.toStdString().data(), response.header.toStdString().size());
    tcp_socket.write(response.getContent(), response.getContentSize());
}
