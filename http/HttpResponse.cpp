#include "HttpResponse.h"
HttpResponse::HttpResponse (){

}

void HttpResponse::append(const QString& s) {
    response.append(s);
}

HttpResponse::HttpResponse(const int status, const char * server_name, const char * content_type, const size_t content_length, const char * content) {
    QString response;
    if (status == 400) {
        response = QString("HTTP/1.1 400 Bad Request\n");
    } else {
        response = QString("HTTP/1.1 200 OK\n");
    }
    response.append("Server: ");
    response.append(server_name);
    response.append("\n");
    response.append("Content-Type: ");
    response.append(content_type);
    response.append("\n");
    response.append("Content-Length: ");
    response.append(QString::number(content_length));
    response.append("\n");
    //response.append("Connection: keep-alive\n");
    response.append("Connection: close\n");
    response.append("\n");

    if (status == 400) {
        response.append("\n");
    }
    this->header = response;
    this->content = content;
    this->content_length = content_length;
}

const char * HttpResponse::getContent() const {
    return content;
}

int HttpResponse::getContentSize() const{
    return content_length;
}
