#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <vector>
#include <QStringList>
#include <QTextStream>
#include <iostream>

class HttpRequest {
public:
    QString requestString;
    bool valid;
    QString method;
    QString version;
    QString uri;
    QString data;
    std::vector<QStringList> headers;
    int build();

    HttpRequest();
    void append(const QString&);
    bool isValid() const;
    std::string toString() const;
};

#endif // HTTPREQUEST_H
