#include "tcp/TcpServer.h"
#include "tcp/Socket.h"

using std::cout;
using std::endl;

TcpServer::TcpServer() : epoll(EpollWrap::getInstance()) {
    cout << "Creating TCP server.\n";
}

TcpServer::~TcpServer() {
}

void TcpServer::addPort(int port, std::function<void(TcpSocket &, EventType)> newData) {
    cout << "Adding tcp port: " << port << endl;

    ServerSocket socket(port, bind(&TcpServer::connectionHandler, this, newData, std::placeholders::_2, std::placeholders::_1));
    int x = socket.getfd();
    auto added = serverSockets.emplace(std::make_pair(x, std::move(socket)));
    callbacks.emplace(std::pair<int, Socket&>(x, added.first->second));

    cout << "Added port " << port << " to epoll\n";
}

void TcpServer::connectionHandler(std::function<void(TcpSocket &, EventType)> newData, ServerSocket &socket, uint32_t event) {
    int fd = socket.getfd();
    if (event & EPOLLRDHUP) {
        callbacks.erase(fd);
        serverSockets.erase(fd);
        return;
    }
    if (event & EPOLLERR) {
        cout << "Error on TCPsocket\n";
        serverSockets.erase(fd);
        callbacks.erase(fd);
        return;
    }
    if (event & EPOLLIN) {
        cout << "New connection\n";
        while (1) {
            sockaddr in_addr;
            socklen_t in_len =  sizeof in_addr;
            FD infd(accept(fd, &in_addr, &in_len));
            if (infd.getfd() == -1){
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) break;
                else {
                    cout << "accept error\n";
                    break;
                }
            }
            // Make the incoming socket non-blocking and add it to the epoll.
            try {
                TcpSocket socket(std::move(infd), bind(&TcpServer::dataHandler, this, newData, std::placeholders::_2, std::placeholders::_1));
                int newFd = socket.getfd();
                auto added = tcpSockets.emplace(std::make_pair(newFd, std::move(socket)));
                callbacks.emplace(std::pair<int, Socket&>(newFd, added.first->second));
            } catch (std::exception& e) {
                cout << "Exception happened while creating TcpSocket\n";
                continue;
            }
        }
    }
}

void TcpServer::dataHandler(std::function<void(TcpSocket &, EventType)> dataHandler, TcpSocket &socket, uint32_t event) {
    int fd = socket.getfd();
    if (event & EPOLLRDHUP) {
        dataHandler(socket, HUP);
        callbacks.erase(fd);
        tcpSockets.erase(fd);
        return;
    }
    if (event & EPOLLERR) {
        dataHandler(socket, ERROR);
        callbacks.erase(fd);
        tcpSockets.erase(fd);
        return;
    }
    if (event & EPOLLIN) {
        dataHandler(socket, NEWDATA);
    }
    if (event & EPOLLOUT) {
        socket.flush();
    }
}

void TcpServer::start() {
    running = true;
    cout << "Epoll starts listening.\n";
    epoll_event events[epoll.MAX_EVENTS];
    for (int n; (n = epoll.wait(events)) >= 0 && running;) {
        cout << "EPOLL EVENTS: " << n << endl;
        cout << "==============================\n";
        for (int i = 0; i < n; i++) {
            cout << "descriptor: " << events[i].data.fd << ". Events:\n";
            if (events[i].events & EPOLLIN)
                cout << "EPOLLIN" << endl;
            if (events[i].events & EPOLLOUT)
                cout << "EPOLLOUT" << endl;
            if (events[i].events & EPOLLERR)
                cout << "EPOLLERR" << endl;
            if (events[i].events & EPOLLRDHUP)
                cout << "EPOLLRDHUP" << endl;
            cout << events[i].data.fd << " " << events[i].events << endl;
            std::map<int, Socket&>::iterator it = callbacks.find(events[i].data.fd);

            assert(it != callbacks.end());
            it->second.handle(events[i].events);
        }
        cout << "==============================\n";
    }
}

void TcpServer::stop() {
    running = false;
}
