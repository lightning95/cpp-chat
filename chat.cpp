#include "chat.h"

using std::cout;
using std::endl;

Chat::Chat(const QString &head_name, const QString &foot_name,const QString &forms_name)
    : head(readFile(head_name))
    , foot(readFile(foot_name))
    , forms(readFile(forms_name)){
}

void Chat::start() {
    http.start();
}

void Chat::stop() {
    http.stop();
}

void Chat::addPort(int port) {
    http.addPort(port, bind(&Chat::request, this, std::placeholders::_1, std::placeholders::_2));
    cout << "Port " << port << " added." << endl;
}

QString Chat::readFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cout << "Can't open " << filename.toStdString() << endl;
        return NULL;
    }
    QByteArray total;
    QByteArray line;
    while (!file.atEnd()) {
        line = file.read(1024);
        total.append(line);
    }
    return QString(total);
}

QByteArray Chat::readBinFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return NULL;
    QByteArray total;
    QByteArray line;
    while (!file.atEnd()) {
        line = file.read(1024);
        total.append(line);
    }
    return total;
}

void Chat::safeString(QString &s) {
    static const QChar c1[] = {'&', '\'', '\"', '/', '>', '<', '\\'};
    static const QString c2[] = {"&amp;", "&#39;", "&quot;", "&#47;", "&gt;", "&lt", "&#42;"};

    QString res;
    for (int i = 0; i < s.length(); ++i){
        auto f = std::find(std::begin(c1), std::end(c1), s.at(i));
        if (f == std::end(c1))
            res += s.at(i);
        else
            res += c2[f - std::begin(c1)];
    }
    s = res;
}

void Chat::sendJquery(HttpSocket& socket) {
    QString jquery(readFile(":/html/jquery.js"));
    socket.write(HttpResponse(200, "Chat", "text/javascript", jquery.toStdString().size(), jquery.toStdString().data()));
}

void Chat::sendIcon(HttpSocket& socket) {
    QByteArray icon(readBinFile(":/html/favicon.ico"));
    socket.write(HttpResponse(200, "Chat", "image/x-icon", icon.size(), icon.data()));
}

void Chat::sendMessages(HttpSocket& socket){
    QString html;
    while (messages.size() > 50)
        messages.pop_front();

    for (std::deque<QString>::reverse_iterator i = messages.rbegin(); i != messages.rend(); i++) {
        html += *i;
        html += "<br>";
    }
    //(const int status, const char * server_name, const char * content_type, const size_t content_length, const char * content)
    socket.write(HttpResponse(200, "Chat", "text/plain", html.toStdString().size(), html.toStdString().data()));
}

void Chat::request(HttpRequest& http_request, HttpSocket& socket) {
    cout << "start processing\n";
    if (http_request.isValid() == 0)                    socket.write(HttpResponse(400, "Chat", "text/html", 0, "")); // send400
    else if (http_request.uri == "/favicon.ico")        sendIcon(socket);
    else if (http_request.uri == "/jquery.js")          sendJquery(socket);
    else if (http_request.uri.mid(0, 9) == "/messages") sendMessages(socket);
    else {
        QString name = "Anonymous";
        for (size_t i = 0; i < http_request.headers.size(); i++)
            if (http_request.headers[i].size() > 1 && http_request.headers[i].at(0) == "Cookie:" &&
                http_request.headers[i].at(1).mid(0, 5) == "name=") {
                    name = http_request.headers[i].at(1).mid(5, http_request.headers[i].at(1).length() - 5);
                    for (int j = 2; j < http_request.headers[i].size(); j++)
                        name += " " + http_request.headers[i].at(j);
            }

        safeString(name);
        if (http_request.method == "POST") {
            QString message;
            message = http_request.data;
            safeString(message);
            if (message.mid(0, 8) == "message=")
                messages.push_back(name + ": " + message.mid(8, message.length() - 8));
        }

        QString html;
        QString hello = "Hello, " + name + "!</br>";
        html = head + hello + forms + foot;
        socket.write(HttpResponse(200, "Chat", "text/html", html.toStdString().size(), html.toStdString().data()));
    }

    cout << "finish processing\n";
}
