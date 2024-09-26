#include "PassCommand.hpp"
#include <iostream>

void PassCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server& server) {
     (void) server;
    if (!clients[clientFd]->getConnect()) {
        if (args.size() >= 2 && args[1] == _password) {
            clients[clientFd]->messageSend("\033[32mAuthentification réussie\r\n\033[0m");
            clients[clientFd]->setConnect(true);
        } else {
            clients[clientFd]->messageSend("\033[31mÉchec de l'authentification\r\n\033[0m");
        }
    }
}
