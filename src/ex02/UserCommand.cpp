#include "UserCommand.hpp"


void UserCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server &server) {
     (void) server;
     if (args.size() < 5) {
        clients[clientFd]->messageSend("461 USER :Not enough parameters\r\n");
        return;
    }

    clients[clientFd]->setHostName(args[1]);
    clients[clientFd]->setRealName(args[4]); 

    if (!clients[clientFd]->getUserName().empty()) {
        clients[clientFd]->messageSend("001 " + clients[clientFd]->getUserName() + " :Welcome to the IRC server\r\n");
        clients[clientFd]->messageSend("002 " + clients[clientFd]->getUserName() + " :Your host is localhost\r\n");
        clients[clientFd]->messageSend("003 " + clients[clientFd]->getUserName() + " :This server was created for testing\r\n");
        clients[clientFd]->messageSend("004 " + clients[clientFd]->getUserName() + " localhost 1.0 i\r\n");
    }
}