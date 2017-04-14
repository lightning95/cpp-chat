#ifndef CHAT_H
#define CHAT_H

#include <QObject>
#include <QUrl>
#include <QFile>

#include "library/HttpResponse.h"
#include "library/HttpRequest.h"
#include "library/HttpSocket.h"
#include "library/HttpServer.h"

#include <iostream>
#include <fstream>

#include <algorithm>
#include <vector>
#include <deque>

class Chat{
    QString head;
    QString foot;
    QString forms;
    std::deque<QString> messages;
    HttpServer http;

    QString readFile(const QString&);
    QByteArray readBinFile(const QString&);
    static void safeString(QString&);
    void sendJquery(HttpSocket& socket);
    void sendIcon(HttpSocket& socket);
    void sendMessages(HttpSocket& socket);
    void request(HttpRequest& http_request, HttpSocket& socket);

public:
    Chat(const QString&, const QString&, const QString&);
    void start();
    void addPort(int);
    void stop();
};

#endif // CHAT_H
