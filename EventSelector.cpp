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

    for (; !tcp_proxy::SHUT_DOWN; ) {

        int             fd, fdIndexInSelector;
        struct pollfd   selectedFds[1024];

        for (fd = 0, fdIndexInSelector = 0; fd <= maxFd; ++fd) {
            if (fdStorage[fd]) {
                selectedFds[fdIndexInSelector].fd = fd;
                if (fdStorage[fd]->wantRead()) {
                    selectedFds[fdIndexInSelector].events |= POLLIN;
                }
                if (fdStorage[fd]->wantWrite()) {
                    selectedFds[fdIndexInSelector].events |= POLLOUT;
                }
                fdIndexInSelector++;
            }
        }
        int result = poll(selectedFds, fdIndexInSelector, -1);
        if (result < 0) {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        if (result > 0) {
            for (fd = 0; fd < fdIndexInSelector; ++fd) {
                if (!fdStorage[selectedFds[fd].fd])
                    continue;

                bool read = selectedFds[fd].revents & POLLIN;
                bool write = selectedFds[fd].revents & POLLOUT;
                if (read || write) {
                    fdStorage[selectedFds[fd].fd]->handle(read, write);
                }
                selectedFds[fd].events = 0;
                selectedFds[fd].revents = 0;
            }
        }
    }
}
