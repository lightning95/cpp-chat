QT       += core

QT       -= gui

TARGET = chat
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
CONFIG += address_sanitizer

TEMPLATE = app


SOURCES += main.cpp \
    http/HttpServer.cpp \
    tcp/TcpServer.cpp \
    tcp/TcpSocket.cpp \
    http/HttpRequest.cpp \
    http/HttpSocket.cpp \
    http/HttpResponse.cpp \
    library/EpollWrap.cpp \
    chat.cpp \
    library/FD.cpp

HEADERS += \
    http/HttpServer.h \
    tcp/TcpServer.h \
    tcp/TcpSocket.h \
    http/HttpRequest.h \
    http/HttpSocket.h \
    http/HttpResponse.h \
    library/EpollWrap.h \
    chat.h \
    library/FD.h

RESOURCES += \
    html/Resources.qrc
