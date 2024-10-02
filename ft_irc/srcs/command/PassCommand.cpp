#include "PassCommand.hpp"
#include <iostream>

void PassCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server& server) {
    (void) server;
    if (args.size() >= 2 && args[1] == _password) {
        clients[clientFd]->messageSend("001 " + clients[clientFd]->getUserName() + " :Authentication successful\r\n");
        clients[clientFd]->setConnect(true);
    } else
        clients[clientFd]->messageSend("464 :Password incorrect\r\n");
}
