#include "PrivmsgCommand.hpp"
#include "Client.hpp"
#include <sstream>
#include <iostream>

void PrivmsgCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server &server) {
    (void)server;
    if (args.size() < 3) {
        clients[clientFd]->messageSend("461 PRIVMSG :Not enough parameters\r\n");
        return;
    }

    std::string recipients = args[1];
    std::string message = args[2];

    std::vector<std::string> recipientList = splitString(recipients, ',');

    for (size_t i = 0; i < recipientList.size(); ++i) {
        std::string recipient = trim(recipientList[i]);

        bool userFound = false;

        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            
            std::cout << "[PRIVINFO] " << recipient << " :"  << message << std::endl;
            
            if (it->second->getUserName() == recipient) {
                it->second->messageSend(clients[clientFd]->getUserName() + " PRIVMSG " + recipient + " :" + message + "\r\n");
                if (clients[clientFd]->getUserName() != recipient)
                    clients[clientFd]->messageSend(clients[clientFd]->getUserName() + " PRIVMSG " + recipient + " :" + message + "\r\n");
                userFound = true;
                break;
            }
        }

        if (recipient[0] == '#') {
            for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->first != clientFd && it->second->isInChannel(recipient)) {
                    it->second->messageSend(":" + clients[clientFd]->getUserName() + " PRIVMSG " + recipient + " :" + message + "\r\n");
                }
                std::cout << "\033[31m[DEBUG] liste des channel : " << clients[clientFd]->getChannelName() << "\033[0m" << std::endl;
            }
            userFound = true;
        }

        if (!userFound) {
            clients[clientFd]->messageSend("401 " + recipient + " :No such nick/channel\r\n");
        }
    }
}
