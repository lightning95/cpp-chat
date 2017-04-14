#include "TcpServer.h"

using std::cout;
using std::endl;

TcpServer::TcpServer() : epoll(EpollWrap::getInstance()) {
    cout << "Creating TCP server.\n";
}

TcpServer::~TcpServer() {
    for (auto e : callbacks) {
        cout << "closing " << e.first << " descriptor.\n";
        e.second(EPOLLRDHUP);
    }
}

void TcpServer::addPort(int port, std::function<void(TcpSocket &, EventType)> newData) {
    cout << "Adding tcp port: " << port << endl;

    FD tcpfd = createAndBind(port);
    makeSocketNonBlocking(tcpfd);
    listenSocket(tcpfd);

//    cout << "Adding port " << port << " to epoll\n";

    callbacks[tcpfd.getfd()] = bind(&TcpServer::connectionHandler, this, newData, tcpfd.getfd(), std::placeholders::_1);
    if (epoll.add(tcpfd.getfd(), EPOLLIN | EPOLLET | EPOLLRDHUP) < 0)
         throw std::runtime_error("Can't add tcpfd to epoll");

    int x = tcpfd.getfd();
    try {
        tcpsockets.push_back(std::move(tcpfd));
    } catch (std::exception& e) {
        callbacks.erase(x);
        epoll.remove(x);
//        epoll.remove(tcpfd.getfd());
        throw std::runtime_error("Can't add tcpfd to tcpvector");
    }

    cout << "Added port " << port << " to epoll\n";
}

void TcpServer::connectionHandler(std::function<void(TcpSocket &, EventType)> newData, int fd, uint32_t event) {
    if (event & EPOLLRDHUP) {
        epoll.remove(fd);
        callbacks.erase(fd);
        return;
    }
    if (event & EPOLLERR) {
        cout << "Error on TCPsocket\n";
        epoll.remove(fd);
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
            try{
                makeSocketNonBlocking(infd);
            } catch (std::runtime_error&e ){
                continue;
            }
            if (epoll.add(infd.getfd(), EPOLLIN | EPOLLET | EPOLLRDHUP) < 0){
                cout << "Can't add to epoll\n";
                continue;
            }
            callbacks[infd.getfd()] = bind(&TcpServer::dataHandler, this, newData, infd.getfd(), std::placeholders::_1);
            int x = infd.getfd();
            try {
                sockets.emplace(std::make_pair(x, TcpSocket(this, std::move(infd))));
            } catch (std::exception& e) {
//                epoll.remove(infd);
                  epoll.remove(x);
                  callbacks.erase(x);
                continue;
            }
        }
    }
}

void TcpServer::dataHandler(std::function<void(TcpSocket &, EventType)> dataHandler, int fd, uint32_t event) {
    if (event & EPOLLRDHUP) {
        auto it = sockets.find(fd);
        assert(it != sockets.end());
        dataHandler(it->second, HUP);
        epoll.remove(fd);
        sockets.erase(fd);
        callbacks.erase(fd);
        return;
    }
    if (event & EPOLLERR) {
        auto it = sockets.find(fd);
        assert(it != sockets.end());
        dataHandler(it->second, ERROR);
        epoll.remove(fd);
        sockets.erase(fd);
        callbacks.erase(fd);
        return;
    }
    if (event & EPOLLIN) {
        auto it = sockets.find(fd);
        assert(it != sockets.end());
        dataHandler(it->second, NEWDATA);
    }
    if (event & EPOLLOUT) {
        auto it = sockets.find(fd);
        assert(it != sockets.end());
        it->second.flush();
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
            callbacks[events[i].data.fd](events[i].events);
        }
        cout << "==============================\n";
    }
}

void TcpServer::stop() {
    running = false;
}

FD TcpServer::createAndBind(int port) {
    addrinfo hints;
    addrinfo *result, *rp;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string portstr = std::to_string(port);
    if (getaddrinfo (NULL, portstr.c_str(), &hints, &result) != 0)
        throw std::runtime_error("Can't getAddrInfo " + std::to_string(port));

    class ai {
        addrinfo *a;
    public :
        ai(addrinfo *a) : a(a) {}
        ~ai() {
            freeaddrinfo(a);
        }
    } tmp(result);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        FD res(socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol));
        if (res.getfd() == -1) continue;
        int t = 1;
        if (setsockopt(res.getfd(), SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)) == -1) {
            cout << "Can't make socket reusable\n";
            continue;
        }
        if (bind(res.getfd(), rp->ai_addr, rp->ai_addrlen) == 0)
            return res;
    }
    throw std::runtime_error("No address succeeded");
}

void TcpServer::makeSocketNonBlocking(FD const& fd) {
    int flags = fcntl (fd.getfd(), F_GETFL, 0);
    if (flags == -1) //return -1;
        throw std::runtime_error("Can't make socket nonblocking");
    flags |= O_NONBLOCK;
    if (fcntl (fd.getfd(), F_SETFL, flags) == -1) //return -1;
        throw std::runtime_error("Can't make socket nonblocking");
}

void TcpServer::listenSocket(FD const& fd){
    if (listen(fd.getfd(), QUEUE_SIZE) == -1)
        throw std::runtime_error("Listen error");
}
