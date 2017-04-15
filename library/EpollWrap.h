#ifndef EPOLLWRAP_H
#define EPOLLWRAP_H

#include <map>
#include <unistd.h>
#include <assert.h>
#include <sys/epoll.h>
#include <iostream>
#include <functional>
#include <memory>

#include "library/FD.h"

class EpollWrap {
public:
    static EpollWrap &getInstance() {
        static EpollWrap s;
        return s;
    }
private:
    bool running;
    FD epollfd;
    EpollWrap();

public:
    static int const MAX_EVENTS = 50;

    int wait(epoll_event [MAX_EVENTS]);

    ~EpollWrap();
    int add(int, uint32_t); //file descriptor to add, function to call when event
                                                           //happens(gets fd and events) and events.
    int remove(int);
    int modify(int, uint32_t);
};

#endif // EPOLLWRAP_H
