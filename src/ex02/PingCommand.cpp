// PingCommand.cpp

#include "PingCommand.hpp"
#include <pthread.h>
#include <unistd.h>  // pour sleep
#include <map>

void PingCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args) {
    if (args.size() >= 2 && !args[1].empty()) {
        std::cout << "\033[34m[DEBUG] le client " << clientFd << " ping " << args[1] << "\033[0m"<< std::endl;
        clients[clientFd]->messageSend("PONG " + args[1] + "\r\n");
    } else {
        clients[clientFd]->messageSend("421 PING :No origin specified\r\n");
    }
}