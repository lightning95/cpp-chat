#include "HttpRequest.h"

using std::cout;
using std::endl;

HttpRequest::HttpRequest() {
    valid = 0;
}

int HttpRequest::build() {
    if (requestString.size() == 0) {
        return -1;
    }
    //cout << "BUILD: " << requestString.toStdString() << endl;
    std::vector<QStringList> tokens;
    int l = 0;
    for (int r = requestString.indexOf("\r\n"); r != -1; r = requestString.indexOf("\r\n", l)) {
        tokens.push_back(requestString.mid(l, r - l).split(' '));
        if (l == r) {
            l = r + 2;
            break;
        }
        l = r + 2;
    }
    data = requestString.mid(l);
    if (tokens[0].size() != 3) {
        return -1;
    }
    if (tokens.size() < 2) {
        return -1;
    }
    method = tokens[0].at(0);
    uri = tokens[0].at(1);
    version = tokens[0].at(2);
    for (size_t i = 1; i < tokens.size() && tokens[i].size() != 0 && tokens[i].at(0).length() != 0; i++) {
        headers.push_back(tokens[i]);
    }
    return 0;
}

void HttpRequest::append(const QString& data) {
    cout << "appending" << endl;
    requestString += data;
    if (build() == 0) {
        valid = 1;
    }
}

bool HttpRequest::isValid() const {
    return valid;
}

std::string HttpRequest::toString() const {
    std::string s = method.toStdString() + " " + uri.toStdString() + " " + version.toStdString() + "\r\n";
    for (size_t i = 0; i < headers.size(); i++) {
        for (int j = 0; j < headers[i].size(); j++) {
            s += headers[i].at(j).toStdString();
            if (j < headers[i].size() - 1) {
                s += " ";
            }
        }
        s += "\r\n";
    }
    s += "\r\n";
    s += data.toStdString();
    return s;
}
