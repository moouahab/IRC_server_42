#include "Logger.hpp"

void Logger::log(const std::string& message) {
    std::ofstream logFile("server.log", std::ios::app);
    if (logFile.is_open()) {
        // Ajouter un timestamp
        std::time_t now = std::time(NULL);
        char buf[64];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        logFile << "[" << buf << "] " << message << std::endl;
        logFile.close();
    }
}