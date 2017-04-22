#include "FD.h"
#include "EpollWrap.h"

FD::FD():fd(NONE) {
}

FD::FD(int fd):fd(fd) {
}

FD::~FD() {
    if (fd == NONE) return;
    if (close(fd) < 0)
        std::cerr << "Can't close\n";
}

int FD::getfd() const{
    return fd;
}

FD::FD(FD&& rhs): fd(rhs.fd){
    rhs.fd = NONE;
}

FD& FD::operator=(FD&& rhs){
    swap(rhs);
    return *this;
}

void FD::swap(FD& other){
    std::swap(fd, other.fd);
}
