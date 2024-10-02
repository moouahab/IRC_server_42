#include "UserCommand.hpp"
#include "Server.hpp"


void UserCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server &server) {
    if (args.size() < 5) {
        clients[clientFd]->messageSend("461 USER :Not enough parameters\r\n");
        return;
    }

    Client* client = clients[clientFd];

    if (client->isRegistered()) {
        client->messageSend("462 :You are already registered\r\n");
        return;
    }

    client->setHostName(args[1]);
    client->setRealName(args[4]);

    if (!client->getUserName().empty() && client->getConnect()) {
        // Envoyer les messages de bienvenue
        client->messageSend("001 " + client->getUserName() + " :Welcome to the IRC server\r\n");
        client->messageSend("002 " + client->getUserName() + " :Your host is " + server.getServerName() + "\r\n");
        client->messageSend("003 " + client->getUserName() + " :This server was created for testing\r\n");
        client->messageSend("004 " + client->getUserName() + " " + server.getServerName() + " 1.0 i\r\n");
    }
}
