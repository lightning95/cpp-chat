QT       += core

QT       -= gui

TARGET = chat
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11
CONFIG += address_sanitizer

TEMPLATE = app


SOURCES += main.cpp \
    library/HttpServer.cpp \
    library/TcpServer.cpp \
    library/TcpSocket.cpp \
    library/HttpRequest.cpp \
    library/HttpSocket.cpp \
    library/HttpResponse.cpp \
    library/EpollWrap.cpp \
    chat.cpp \
    library/FD.cpp

HEADERS += \
    library/HttpServer.h \
    library/TcpServer.h \
    library/TcpSocket.h \
    library/HttpRequest.h \
    library/HttpSocket.h \
    library/HttpResponse.h \
    library/EpollWrap.h \
    chat.h \
    library/FD.h

RESOURCES += \
    html/Resources.qrc
