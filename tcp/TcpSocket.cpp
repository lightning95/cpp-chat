#include "TcpSocket.h"

using std::cout;
using std::endl;

TcpSocket::TcpSocket(TcpServer* serv, FD&& socket)
    : server(serv)
    , buffersize(0)
    , fd(std::move(socket)) {
    cout << "Creating TCP socket: " << fd.getfd() << endl;
}

TcpSocket::TcpSocket(TcpSocket&& sock)
    : server(sock.server)
    , buffersize(0)
    , fd(std::move(sock.fd)) {
}

TcpSocket::~TcpSocket() {
    cout << "Destructing socket: " << fd.getfd() << endl;
}

int TcpSocket::read(QString& str) {
    char data[BUFFER_SIZE];
    int size;
    while (1) {
        size = ::read(fd.getfd(), data, BUFFER_SIZE);
        cout << size << endl;
        if (size == 0)
            return 0;
        else if (size == -1) {
            if (errno != EAGAIN) {
                cout << "reading failed: " << strerror(errno) << endl;
                str = "";
                return -1;
            } else
                return 1;
        } else {
            data[size] = 0;
            str.append(data);
        }
    }
}

int TcpSocket::write(const char * data, size_t size) {
    if (buffersize + size > BUFFER_SIZE)
        return -1;
    for (size_t i = 0; i < size; i++)
        buffer[buffersize + i] = data[i];
    buffersize += size;
    server->epoll.modify(fd.getfd(), EPOLLOUT | EPOLLIN | EPOLLET | EPOLLRDHUP);
    return 0;
}

void TcpSocket::flush() {
    cout << "flushing " << buffersize << " bytes\n";
    size_t count = 0;
    while (count != buffersize) {
        int written = ::write(fd.getfd(), buffer + count, buffersize - count);
        if (written < 0) {
            if (errno != EAGAIN) {
                cout << "writing failed: " << strerror(errno) << endl;
                return;
            } else
                break;
        } else
            count += written;
    }
    for (size_t i = count; i < buffersize; i++)
        buffer[i - count] = buffer[i];
    if (buffersize == count)
        server->epoll.modify(fd.getfd(), EPOLLIN | EPOLLET | EPOLLRDHUP);
    cout << count << " bytes flushed\n";
    buffersize -= count;
}

TcpSocket &TcpSocket::operator=(TcpSocket &&sock) {
    std::swap(fd, sock.fd);
    std::swap(server, sock.server);
    for (size_t i = 0; i < buffersize; i++) {
        std::swap(buffer[i], sock.buffer[i]);
    }
    std::swap(buffersize, sock.buffersize);
    return *this;
}
