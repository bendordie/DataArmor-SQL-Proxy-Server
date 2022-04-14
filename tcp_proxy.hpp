#ifndef __TCP_PROXY_HPP__
#define __TCP_PROXY_HPP__

#include <fstream>
#include <chrono>
#include <iomanip>
#include <csignal>
#include "Session.hpp"
#include "utilities.hpp"

class Session;

namespace tcp_proxy {

    #define BUFFER_SIZE 4096
    #define SERVER_BACKLOG 16

    extern bool SHUT_DOWN; // пока значение false сервер (а именно EventSelector) продолжает работу, иначе - экстренное завершение

    void          signalHandler(int status);
    void          writeToSyslog(const Session *session, const char *message);
}

#endif //__TCP_PROXY_HPP__
