#ifndef __PROXY_SERVER_HPP__
#define __PROXY_SERVER_HPP__

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <list>
#include "FdHandler.hpp"
#include "EventSelector.hpp"
#include "Session.hpp"
#include "TcpBridge.hpp"

class Session;

class TcpBridge;

class EventSelector;

class ProxyServer : public FdHandler {

    using string = std::string;

public:

    ~ProxyServer();
    ProxyServer(const ProxyServer& other) = delete;
    ProxyServer& operator=(const ProxyServer& other) = delete;

    static ProxyServer*   init(EventSelector *eventSelector, int localPort, const string& forwardIp, int forwardPort);
    void                  removeSession(Session *session);
    void                  removeTcpBridge(TcpBridge *bridge);

private:

    ProxyServer(EventSelector *eventSelector, int listenSocket, const string& forwardIp, int forwardPort);

    void    handle(bool read, bool write) override;

    string                 forwardIp;
    int                    forwardPort;
    EventSelector          *eventSelector;
    std::list<Session*>    sessions;
    std::list<TcpBridge*>  tcpBridges;

};

#endif //PROXY_SERVER_HPP
