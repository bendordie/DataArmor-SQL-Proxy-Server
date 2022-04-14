#include "Session.hpp"

using std::string;

Session::Session(ProxyServer *master, int fd, const string &clientIp, TcpBridge *tcpBridge)
    : FdHandler(fd, true), clientIp(clientIp), masterServer(master), tcpBridge(tcpBridge) {}

Session::~Session() {
    masterServer->removeTcpBridge(tcpBridge);
}


void Session::handleRequest() {

    long   bytesRead;

    bytesRead = recv(getFd(), buffer, BUFFER_SIZE - 1, 0);
    buffer[bytesRead] = '\0';
    if (bytesRead < 0) {
        std::cerr << clientIp << ": Error: " << strerror(errno) << ". Connection has been closed" << std::endl;
        masterServer->removeSession(this);
        return ;
    }
    if (bytesRead == 0) {
        std::cerr << clientIp << ": Client has closed the connection" << std::endl;
        masterServer->removeSession(this);
        return ;
    }

    int   offset = MYSQL_PACKET_HEADER_LEN + 1;
    if (bytesRead >= offset &&
    utils::isBufferPrintable(&buffer[offset], bytesRead - offset)) {
        tcp_proxy::writeToSyslog(this, &buffer[offset]);
    }
    tcpBridge->setDataToSent(buffer);
    tcpBridge->setDataToSentSize(bytesRead);
}

void Session::handleResponse() {

    const char *data = tcpBridge->getData();
    size_t     dataSize = tcpBridge->getSizeOfData();
    ssize_t    bytesSent;

    bytesSent = send(getFd(), data, dataSize, 0);
    if (bytesSent < 0) {
        std::cerr << clientIp << ": Error: " << strerror(errno) << ". Connection has been closed" << std::endl;
        masterServer->removeSession(this);
    }
    tcpBridge->setDataIsHandled();
}

void Session::handle(bool read, bool write) {

    if (tcpBridge && tcpBridge->isErrorOccurred()) {
        std::cerr << clientIp << ": Connection has been closed due to error on TcpBridge" << std::endl;
        masterServer->removeSession(this);
        return;
    }
    if (read) {
        handleRequest();
    }
    if (write) {
        handleResponse();
    }
}

bool Session::wantWrite() const {

    if (tcpBridge->isDataNeedsHandling())
        return true;

    return false;
}

const std::string &Session::getClientIpString() const { return clientIp; }