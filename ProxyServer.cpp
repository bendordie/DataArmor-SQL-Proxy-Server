#include "ProxyServer.hpp"

using std::string;

ProxyServer::ProxyServer(EventSelector *eventSelector, int listenSocket, const string& forwardIp, int forwardPort)
    : FdHandler(listenSocket, true), forwardIp(forwardIp), forwardPort(forwardPort),
    eventSelector(eventSelector) {

    eventSelector->add(this);
}

ProxyServer::~ProxyServer() {

    for (auto session : sessions) {
        eventSelector->remove(session);
    }
    for (auto tcpBridge : tcpBridges) {
        eventSelector->remove(tcpBridge);
    }
    eventSelector->remove(this);
}

ProxyServer *ProxyServer::init(EventSelector *eventSelector, int localPort, const string& forwardIp, int forwardPort) {

    int                 listen_socket, option, result;
    struct sockaddr_in  addr;

    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1)
        return nullptr;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(localPort);

    option = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // TODO: delete

    result = bind(listen_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (result == -1)
        return nullptr;

    result = listen(listen_socket, SERVER_BACKLOG);
    if (result == -1)
        return nullptr;

    TcpBridge *bridge = TcpBridge::init(forwardIp, forwardPort); // checking forward connection
    if (!bridge)
        return nullptr;

    delete bridge;

    return new ProxyServer(eventSelector, listen_socket, forwardIp, forwardPort);
}

void ProxyServer::handle(bool read, bool write) {

    (void)write;
    if (!read)
        return;
    int                 clientSocket;
    struct sockaddr_in  addr;
    socklen_t           len = sizeof(addr);

    clientSocket = accept(this->getFd(), (struct sockaddr*)&addr, &len);
    if (clientSocket == -1)
        return;

    char clientIp[INET6_ADDRSTRLEN];
    inet_ntop(addr.sin_family, &addr.sin_addr, clientIp, sizeof(clientIp));

    fcntl(clientSocket, F_SETFL, O_NONBLOCK);

    auto   tcpBridge = TcpBridge::init(forwardIp, forwardPort);
    if (!tcpBridge)
        return;
    tcpBridges.push_front(tcpBridge);
    eventSelector->add(tcpBridge);

    auto   *session = new Session(this, clientSocket, clientIp, tcpBridge);
    sessions.push_front(session);
    eventSelector->add(session);
}

void ProxyServer::removeSession(Session *session) {

    eventSelector->remove(session);
    for (auto it = sessions.begin(); it != sessions.end(); ++it) { // TODO: error due to erasing
        if ((*it) == session) {
            sessions.erase(it);
            delete *it;
            return;
        }
    }
}

void ProxyServer::removeTcpBridge(TcpBridge *bridge) {
    eventSelector->remove(bridge);
    for (auto it = tcpBridges.begin(); it != tcpBridges.end(); ++it) { // TODO: error due to erasing
        if ((*it) == bridge) {
            tcpBridges.erase(it);
            delete *it;
            return;
        }
    }
}

