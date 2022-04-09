#include "EventSelector.hpp"

EventSelector::EventSelector() : fdStorage(nullptr) {}

EventSelector::~EventSelector() { delete [] fdStorage; }

void EventSelector::add(FdHandler *handler) {

    int i;
    int fd = handler->getFd();

    if (!fdStorage) {
        fdStorageLength = fd > 15 ? fd + 1 : 16;
        fdStorage = new FdHandler*[fdStorageLength];
        for (i = 0; i < fdStorageLength; i++) {
            fdStorage[i] = nullptr;
        }
        maxFd = -1;
    }
    if (fdStorageLength <= fd) {
        FdHandler   **tmp = new FdHandler*[fd + 1];
        for (i = 0; i <= fd; i++)
            tmp[i] = i < fdStorageLength ? fdStorage[i] : nullptr;
        fdStorageLength = fd + 1;
        delete [] fdStorage;
        fdStorage = tmp;
    }
    if (fd > maxFd) {
        maxFd = fd;
    }
    fdStorage[fd] = handler;
}

bool EventSelector::remove(FdHandler *handler) {

    int fd = handler->getFd();

    if (fd >= fdStorageLength || fdStorage[fd] != handler)
        return false;
    fdStorage[fd] = 0;
    if (fd == maxFd) {
        for (; maxFd >= 0 && !fdStorage[maxFd]; --maxFd) {}
    }

    return true;
}

void EventSelector::run() {

    for (; !SHUT_DOWN; ) {

        int     i;
        fd_set  readSet, writeSet;

        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        for (i = 0; i <= maxFd; ++i) {
            if (fdStorage[i]) {
                if (fdStorage[i]->wantRead()) {
                    FD_SET(i, &readSet);
                }
                if (fdStorage[i]->wantWrite()) {
                    FD_SET(i, &writeSet);
                }
            }
        }
        int result = select(maxFd + 1, &readSet, &writeSet, 0, 0);
        if (result < 0) {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        if (result > 0) {
            for (i = 0; i <= maxFd; ++i) {
                if (!fdStorage[i])
                    continue;
                bool read = FD_ISSET(i, &readSet);
                bool write = FD_ISSET(i, &writeSet);
                if (read || write) {
                    fdStorage[i]->handle(read, write);
                }
            }
        }
    }
}
