#ifndef __TCP_BRIDGE_HPP__
#define __TCP_BRIDGE_HPP__

#include <string>
#include <iostream>
#include <string.h>
#include <strings.h>
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
    bool                wantWrite() const override;
    void                sendMessage();
    const char*         getData() const;
    size_t              getSizeOfData() const;
    bool                isDataNeedsHandling() const;
    bool                isErrorOccurred() const;

    void                setDataIsHandled();
    void                setDataToSent(const char *dataToSent);
    void                setDataToSentSize(size_t dataToSentSize);

private:

    char         dataBuffer[BUFFER_SIZE];
    long         dataSize;
    bool         dataNeedsHandling;
    bool         errorOccurred;
    const char   *dataToSent;
    long         dataToSentSize;

};

#endif //__TCP_BRIDGE_HPP__
