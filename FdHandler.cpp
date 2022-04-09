#include "FdHandler.hpp"

FdHandler::FdHandler(int fd, bool own) : fd(fd), ownFd(own) {}

FdHandler::~FdHandler() {
    if (ownFd)
        close(fd);
}

int FdHandler::getFd() const { return fd; }

bool FdHandler::wantRead() const { return true; }

bool FdHandler::wantWrite() const { return false; }
