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

class ProxyServer;

class Session : public FdHandler {

    friend class ProxyServer;
    friend class TcpBridge;

    Session(ProxyServer *masterServer, int fd, const std::string &clientIp);
    ~Session();
    Session(const Session& other) = delete;
    Session& operator=(const Session& other) = delete;

public:

    const std::string&   getClientIpString() const;

private:

    std::string         clientIp;
    char                *clientDataStorage;
    size_t              sizeOfStoredData;
    size_t              storageSize;
    ProxyServer         *masterServer;
    TcpBridge           *tcpBridge;
    bool                dataIsFullReceived;

    void                handle(bool read, bool write) override;
    bool                wantRead() const override;
    bool                wantWrite() const override;
    void                handleRequest();
    void                handleResponse();
    bool                receiveDataFromClient(char *buffer, size_t &bytesRead);
    bool                initTcpBridge();
    void                storeDataFromClient(const char *buffer, size_t bytesRead);


protected:

//    void                writeToSyslog(int syslogLevel, const char *message) const;


};

#endif //__SESSION_HPP__
