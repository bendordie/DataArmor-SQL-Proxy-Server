#include <unistd.h>
#include <signal.h>
#include "tcp_proxy.hpp"
#include "ProxyServer.hpp"

bool SHUT_DOWN = false;

void signalHandler(int status) {

    if (WIFSIGNALED(status))
    {
        if (status == SIGTERM || status == SIGHUP) {
            SHUT_DOWN = true;
            std::cout << "Terminated. Exit..." << std::endl;
        }
    }
    else
        exit(WEXITSTATUS(status));
}

std::string tcp_proxy::getCurrentTimeAndDateString()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

void tcp_proxy::writeToSyslog(const Session *session, const char *message) {

    std::ofstream     logFile("tcp_proxy.log", std::ios_base::app);
    const std::string &timestamp = getCurrentTimeAndDateString();

    if (!logFile.is_open()) {
        std::cerr << "Warning: Can't open or create log file" << std::endl;
        return ;
    }
    logFile << "ip: " << session->getClientIpString() << " time: " << timestamp << " request: "<< message << std::endl;
    logFile.close();
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        std::cerr << "Usage: tcpproxy <local_port> <forward_ip> <forward_port>" << std::endl;
        exit(1);
    }

    std::string forwardIp = argv[2];
    int         localPort, forwardPort;

    try {
        localPort = std::strtol(argv[1], nullptr, 10);
        forwardPort = std::strtol(argv[3], nullptr, 10);
    } catch (...) {
        std::cerr << "Error: Invalid port value" << std::endl;
    }

    if (localPort == forwardPort) {
        std::cerr << "Error: <local_port> local port must not be equal to <forward_port>" << std::endl;
        exit(1);
    }

    EventSelector   *eventSelector = new EventSelector;
    ProxyServer     *server =   ProxyServer::init(eventSelector, localPort, forwardIp, forwardPort);

    if (!server) {
        std::cerr << "Error: Can't initialize server: " << strerror(errno) << std::endl;
        exit(1);
    }
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);

    std::cout << "Ready for connections" << std::endl;
    eventSelector->run();
    delete server;
    delete eventSelector;
    std::cout << "Exit" << std::endl;

    return 0;
}

