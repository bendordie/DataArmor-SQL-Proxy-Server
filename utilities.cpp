#include "utilities.hpp"

std::string utils::getCurrentTimeAndDateString()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

bool utils::isBufferPrintable(const char *buffer, long bufferSize) {

    if (!buffer || bufferSize == 0)
        return false;

    for (long i = 0; i < bufferSize; ++i) {
        if (!isprint(buffer[i]))
            return false;
    }

    return true;
}

