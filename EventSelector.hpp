/****************************************************************************************/
/* Класс, задачей которого является отслеживание событий,                               */
/* произошедших на определенных файловых дескрипторах (в рамках сервера - на сокетах).  */
/****************************************************************************************/

#ifndef __EVENT_SELECTOR_HPP__
#define __EVENT_SELECTOR_HPP__

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "FdHandler.hpp"
#include "tcp_proxy.hpp"

class EventSelector {


public:

    EventSelector();
    ~EventSelector();
    EventSelector(const EventSelector &other) = delete;
    EventSelector& operator=(const EventSelector &other) = delete;

    void        add(FdHandler *handler);
    bool        remove(FdHandler *handler);
    void        run();



private:

    FdHandler   **fdStorage;
    int         fdStorageLength;
    int         maxFd;


};

#endif //__EVENTSELECTOR_HPP__
