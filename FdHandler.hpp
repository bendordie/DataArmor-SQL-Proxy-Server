#pragma once
#ifndef __FD_HANDLER_HPP__
#define __FD_HANDLER_HPP__

#include <unistd.h>

class FdHandler {


public:

    FdHandler(int fd = -1, bool own = true);
    virtual ~FdHandler();

    virtual void    handle(bool read, bool write) = 0;
    int             getFd() const;
    virtual bool    wantRead() const;
    virtual bool    wantWrite() const;

private:

    int     fd;
    bool    ownFd;

};

#endif //__FDHANDLER_HPP__
