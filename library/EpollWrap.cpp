#include "EpollWrap.h"
#include "memory.h"

using std::cout;
using std::endl;

EpollWrap::EpollWrap() {
    epollfd = FD(epoll_create1(0));
    if (epollfd.getfd() == -1)
        throw std::runtime_error("Can't create epoll instance");
    cout << "Creating epoll wrap " << epollfd.getfd() << endl;
}

int EpollWrap::wait(epoll_event events[MAX_EVENTS]) {
    int n = epoll_wait(epollfd.getfd(), events, MAX_EVENTS, -1);
    if (n < 0){
        cout << "Epoll_wait failed\n";
        return -1;
    }
    return n;
}

EpollWrap::~EpollWrap() {
    cout << "Deleting epoll wrap " << epollfd.getfd() << endl;
    assert(epollfd.getfd() != -1);
}

int EpollWrap::add(int fd, uint32_t events) {
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epollfd.getfd(), EPOLL_CTL_ADD, fd, &event) == -1)
        return -1;
//    callbacks[fd] = callback;
    cout << "Added " << fd << " descriptor to epoll.\n";
    return 0;
}

int EpollWrap::remove(int fd) {
    cout << "removing from epoll: " << fd << endl;
//    assert(callbacks.find(fd) != callbacks.end());
//    callbacks.erase(fd);
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    if (epoll_ctl(epollfd.getfd(), EPOLL_CTL_DEL, fd, &event) == -1)
        return -1;
    return 0;
}

int EpollWrap::modify(int fd, uint32_t events) {
    cout << "modifying " << fd << " socket\n";
//    assert(callbacks.find(fd) != callbacks.end());
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = events;
    event.data.fd = fd;
    if (epoll_ctl(epollfd.getfd(), EPOLL_CTL_MOD, fd, &event) == -1)
        return -1;
    return 0;
}
