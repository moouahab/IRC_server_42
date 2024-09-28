// Logger.hpp
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <ctime>

class Logger {
public:
    static void log(const std::string& message);
};

#endif // LOGGER_HPP
