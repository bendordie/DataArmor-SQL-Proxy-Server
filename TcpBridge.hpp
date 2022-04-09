#ifndef __TCP_BRIDGE_HPP__
#define __TCP_BRIDGE_HPP__

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "FdHandler.hpp"

#define BUFFER_SIZE 4096

class TcpBridge : public FdHandler {

    using string = std::string;

public:

    TcpBridge(int forwardSocket);
    ~TcpBridge();

    static TcpBridge*   init(const string &forwardIp, int forwardPort);

    void                handle(bool read, bool write) override;
    bool                wantRead() const override;
    bool                sendMessage(const char *msg, size_t msgSize) const;
    const char*         getData() const;
    size_t              getSizeOfData() const;
    bool                isDataNeedsHandling() const;
    bool                isDataFullyReceived() const;
    bool                isErrorOccurred() const;

    void                setDataIsHandled();

private:

    char      dataBuffer[BUFFER_SIZE];
    size_t    dataSize;
    bool      dataNeedsHandling;
    bool      dataIsFullyReceived;
    bool      errorOccurred;

};

#endif //__TCP_BRIDGE_HPP__
