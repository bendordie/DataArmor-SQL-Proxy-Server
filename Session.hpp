/*********************************************/
/* Класс для обработки соединения с клиентом */
/*********************************************/

#ifndef __SESSION_HPP__
#define __SESSION_HPP__

#include <unistd.h>
#include <iostream>
#include <cstring>
#include <list>
#include <sstream>
#include <fstream>
#include <syslog.h>
#include "tcp_proxy.hpp"
#include "ProxyServer.hpp"
#include "FdHandler.hpp"
#include "TcpBridge.hpp"

#define BUFFER_SIZE 4096
#define MYSQL_PACKET_HEADER_LEN 4

class ProxyServer;

class Session : public FdHandler {

    friend class ProxyServer;
    friend class TcpBridge;

    Session(ProxyServer *masterServer, int fd, const std::string &clientIp, TcpBridge *tcpBridge);
    ~Session();
    Session(const Session& other) = delete;
    Session& operator=(const Session& other) = delete;

public:

    const std::string&   getClientIpString() const;

private:

    std::string         clientIp;
    char                buffer[BUFFER_SIZE];
    ProxyServer         *masterServer;
    TcpBridge           *tcpBridge;

    void                handle(bool read, bool write) override;
    bool                wantWrite() const override;
    void                handleRequest();
    void                handleResponse();

};

#endif //__SESSION_HPP__
