#include "Session.hpp"

using std::string;

Session::Session(ProxyServer *master, int fd, const string &clientIp)
    : FdHandler(fd, true), clientIp(clientIp), clientDataStorage(nullptr), sizeOfStoredData(0),
    storageSize(BUFFER_SIZE), masterServer(master), tcpBridge(nullptr), dataIsFullReceived(false) {}

Session::~Session() {
    delete clientDataStorage;
    masterServer->removeTcpBridge(tcpBridge);
}

bool Session::initTcpBridge() {
    const string &forwardIp = masterServer->getForwardIp();
    int          forwardPort = masterServer->getForwardPort();

    tcpBridge = TcpBridge::init(forwardIp, forwardPort);

    return tcpBridge;
}

void Session::storeDataFromClient(const char *buffer, size_t bytesRead) {
    if (sizeOfStoredData + bytesRead <= storageSize) {
        std::memcpy(clientDataStorage, buffer, bytesRead);
    } else {
        storageSize *= 2;
        char *newStorage = new char[storageSize];
        std::memcpy(newStorage, clientDataStorage, sizeOfStoredData);
        std::memcpy(newStorage + sizeOfStoredData, buffer, bytesRead);
        delete clientDataStorage;
        clientDataStorage = newStorage;
    }
    sizeOfStoredData += bytesRead;
}

void Session::handleRequest() {

    if (!clientDataStorage)
        clientDataStorage = new (std::nothrow) char[storageSize];
    if (!clientDataStorage) {
        std::cerr << "Session " << getFd() << ": Error: " << strerror(errno) << ". Connection closed" << std::endl;
        masterServer->removeSession(this);
        return ;
    }

    char   buffer[storageSize+1];
    size_t bytesRead;
    bool   result;

    bytesRead = recv(getFd(), buffer, sizeof(buffer) - 1, 0);
    buffer[bytesRead] = '\0';
    if (bytesRead < 0) {
        std::cerr << "Session " << getFd() << ": Error: " << strerror(errno) << ". Connection closed" << std::endl;
        masterServer->removeSession(this);
        return ;
    }
    if (bytesRead > 0)
        storeDataFromClient(buffer, bytesRead);
    if (!dataIsFullReceived)
        return ;
    tcp_proxy::writeToSyslog(this, clientDataStorage);
    result = initTcpBridge();
    if (!result) {
        masterServer->removeSession(this);
        return ;
    }
    masterServer->addTcpBridgeToSelect(tcpBridge);
    result = tcpBridge->sendMessage(clientDataStorage, sizeOfStoredData);
    if (!result) {
        masterServer->removeSession(this);
        return ;
    }
}

void Session::handleResponse() {

    const char *data = tcpBridge->getData();
    size_t     dataSize = tcpBridge->getSizeOfData();
    ssize_t    bytesSent;

    bytesSent = send(getFd(), data, dataSize, 0);
    if (bytesSent < 0) {
        std::cerr << "Session " << getFd() << ": Error: " << strerror(errno) << ". Connection closed" << std::endl;
        masterServer->removeSession(this);
    }

    if (tcpBridge->isDataFullyReceived()) {
        masterServer->removeSession(this);
        return;
    }
    tcpBridge->setDataIsHandled();
}

void Session::handle(bool read, bool write) {

    if (tcpBridge && tcpBridge->isErrorOccurred()) {
        std::cerr << "Session " << getFd() << ": Connection closed due to error on TcpBridge" << std::endl;
        masterServer->removeSession(this);
        return;
    }
    if (write)
        dataIsFullReceived = true;
    if (read)
        handleRequest();
    if (write && tcpBridge && tcpBridge->isDataNeedsHandling())
        handleResponse();
}

bool Session::wantRead() const {

    if (!tcpBridge)
        return true;
    if (tcpBridge->isDataNeedsHandling())
        return false;

    return !tcpBridge->wantRead();
}

bool Session::wantWrite() const {

    if (!tcpBridge)
        return true;

    return tcpBridge->isDataNeedsHandling();
}

//bool Session::receiveDataFromClient(char *buffer, size_t &bytesRead) {
//
//    bytesRead = recv(getFd(), buffer, sizeof(buffer) - 1, 0);
//    buffer[bytesRead] = '\0';
//    if (bytesRead < 0) {
////        writeToSyslog(LOG_ERR);
//        return false;
//    }
//    if (bytesRead == 0) {
//        dataIsFullReceived = true;
////        writeToSyslog(LOG_ERR, "Client closed the connection"); // TODO:  error mngmt
//        return false;
//    }
//
//    return true;
//}

const std::string &Session::getClientIpString() const { return clientIp; }