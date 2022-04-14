#include "TcpBridge.hpp"

TcpBridge::TcpBridge(int forwardSocket)
   : FdHandler(forwardSocket), dataSize(0), dataNeedsHandling(false), errorOccurred(false),
   dataToSent(nullptr), dataToSentSize(0) {

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

void TcpBridge::sendMessage() {

    long bytesSent;

    bytesSent = send(getFd(), dataToSent, dataToSentSize, 0);
    if (bytesSent < 0) {
        std::cerr << "Error: TcpBridge: " << strerror(errno) << std::endl;
        errorOccurred = true;
    }
    dataToSent = nullptr;
    dataToSentSize = 0;
}

void TcpBridge::setDataToSent(const char *dataToSent) { this->dataToSent = dataToSent; }

void TcpBridge::setDataToSentSize(size_t dataToSentSize) { this->dataToSentSize = dataToSentSize; }

void TcpBridge::handle(bool read, bool write) {

    if (write) {
        sendMessage();
        return;
    }
    if (!read)
        return;

    long bytesRead;

    bzero(dataBuffer, BUFFER_SIZE);
    bytesRead = recv(getFd(), dataBuffer, BUFFER_SIZE - 1, 0);
    dataBuffer[bytesRead] = '\0';
    if (bytesRead < 0) {
        std::cerr << "Error: TcpBridge: " << strerror(errno) << std::endl;
        errorOccurred = true;
        return ;
    }
    dataSize = bytesRead;
    dataNeedsHandling = true;
}

bool TcpBridge::wantRead() const {
    if (!dataNeedsHandling && !errorOccurred)
        return true;

    return false;
}

bool TcpBridge::wantWrite() const {
    if (dataToSent)
        return true;

    return false;
}

const char *TcpBridge::getData() const { return dataBuffer; }

size_t TcpBridge::getSizeOfData() const { return dataSize; }

bool TcpBridge::isDataNeedsHandling() const { return dataNeedsHandling; }

void TcpBridge::setDataIsHandled() { dataNeedsHandling = false; }

bool TcpBridge::isErrorOccurred() const { return errorOccurred; }