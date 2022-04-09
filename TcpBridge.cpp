#include "TcpBridge.hpp"

TcpBridge::TcpBridge(int forwardSocket)
   : FdHandler(forwardSocket), dataSize(0), dataNeedsHandling(false), dataIsFullyReceived(false), errorOccurred(false) {

    bzero(dataBuffer, sizeof(dataBuffer));
}

TcpBridge::~TcpBridge() {}

TcpBridge *TcpBridge::init(const string &forwardIp, int forwardPort) {

    int                 forwardSocket;
    struct sockaddr_in  forwardServerAddr;

    if ((forwardSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error: TcpBridge: " << strerror(errno) << std::endl;
        return nullptr;
    }

    bzero(&forwardServerAddr, sizeof(forwardServerAddr));
    forwardServerAddr.sin_family = AF_INET;
    forwardServerAddr.sin_port = htons(forwardPort);

    if (inet_pton(AF_INET, forwardIp.c_str(), &forwardServerAddr.sin_addr) <= 0) {
        std::cerr << "Error: TcpBridge: " << strerror(errno) << std::endl;
        return nullptr;
    }
    if (connect(forwardSocket, (struct sockaddr*)&forwardServerAddr, sizeof(forwardServerAddr)) < 0) {
        std::cerr << "Error: TcpBridge: " << strerror(errno) << std::endl;
        return nullptr;
    }

    return new TcpBridge(forwardSocket);
}

bool TcpBridge::sendMessage(const char *msg, size_t msgSize) const {

    ssize_t bytesSent;

    bytesSent = send(getFd(), msg, msgSize, 0);
    if (bytesSent < 0) {
        std::cerr << "Error: TcpBridge: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void TcpBridge::handle(bool read, bool write) {

    (void)write;
    if (!read)
        return;

    size_t bytesRead;

    bzero(dataBuffer, BUFFER_SIZE);
    bytesRead = recv(getFd(), dataBuffer, BUFFER_SIZE - 1, 0);
    dataBuffer[bytesRead] = '\0';
    if (bytesRead < 0) {
        std::cerr << "Error: TcpBridge: " << strerror(errno) << std::endl;
        errorOccurred = true;
        return ;
    }
    dataNeedsHandling = true;
    if (bytesRead > 0) {
        dataSize = bytesRead;
        std::cout << dataBuffer << std::endl;
    }
    if (bytesRead == 0) {
        dataIsFullyReceived = true;
        return ;
    }
    if (!dataIsFullyReceived)
        return;
}

bool TcpBridge::wantRead() const {

    if (!dataNeedsHandling && !errorOccurred)
        return true;
    else
        return false;
}

const char *TcpBridge::getData() const { return dataBuffer; }

size_t TcpBridge::getSizeOfData() const { return dataSize; }

bool TcpBridge::isDataNeedsHandling() const { return dataNeedsHandling; }

void TcpBridge::setDataIsHandled() { dataNeedsHandling = false; }

bool TcpBridge::isDataFullyReceived() const { return dataIsFullyReceived; }

bool TcpBridge::isErrorOccurred() const { return errorOccurred; }