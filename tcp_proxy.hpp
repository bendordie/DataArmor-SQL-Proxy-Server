#ifndef __TCP_PROXY_HPP__
#define __TCP_PROXY_HPP__

#include <fstream>
#include <chrono>
#include "Session.hpp"

#define BUFFER_SIZE 4096
#define SERVER_BACKLOG 16

extern bool SHUT_DOWN; // пока значение false сервер (а именно EventSelector) продолжает работу, иначе - экстренное завершение

class Session;

namespace tcp_proxy {

    void          writeToSyslog(const Session *session, const char *message);
    std::string   getCurrentTimeAndDateString();
}

#endif //__TCP_PROXY_HPP__
