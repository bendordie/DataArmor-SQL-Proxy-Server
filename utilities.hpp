#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace utils {

    bool          isBufferPrintable(const char *buffer, long bufferSize);
    std::string   getCurrentTimeAndDateString();
}

#endif //UTILITIES_HPP
