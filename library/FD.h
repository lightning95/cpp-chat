#ifndef FD_H
#define FD_H

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <iostream>

class FD{
    int fd;
public:
    static const int NONE = -1;

    int getfd() const;
    FD(int);
    FD();
    ~FD();

    FD(FD const&) = delete;
    FD& operator =(FD const&)  = delete;

    FD(FD&&);
    FD& operator =(FD&&);

    void swap(FD& other);
};
#endif // FD_H

