#include "JoinCommand.hpp"
#include "Server.hpp"
#include <sstream>

void JoinCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    if (args.size() < 2) {
        clients[clientFd]->messageSend("461 JOIN :Not enough parameters\r\n");
        return;
    }

    std::string channelName = args[1];

    // Vérifier si le canal existe
    Channel *channel = server.getChannel(channelName);
    if (!channel) {
        // Créer le canal s'il n'existe pas
        server.createChannel(channelName, clients[clientFd]);
        channel = server.getChannel(channelName);
    }

    // Ajouter le client au canal
    channel->addClient(clients[clientFd]);
    clients[clientFd]->joinChannel(channel);

    // Envoyer les messages appropriés
    std::string joinMessage = ":" + clients[clientFd]->getUserName() + " JOIN :" + channelName + "\r\n";
    channel->broadcast(joinMessage);

    // Envoyer le topic du canal
    if (!channel->getTopic().empty()) {
        clients[clientFd]->messageSend("332 " + clients[clientFd]->getUserName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
    } else {
        clients[clientFd]->messageSend("331 " + clients[clientFd]->getUserName() + " " + channelName + " :No topic is set\r\n");
    }

    // Lister les utilisateurs du canal
    std::string namesList = "= " + channelName + " :";
    std::set<Client*> channelClients = channel->getClients();
    // Dans JoinCommand.cpp, lors de la construction de la liste des noms
    for (std::set<Client*>::iterator it = channelClients.begin(); it != channelClients.end(); ++it) {
        if (channel->isOperator(*it)) {
            namesList += "@" + (*it)->getUserName() + " ";
        } else {
            namesList += (*it)->getUserName() + " ";
        }
    }
    namesList += "\r\n";
    clients[clientFd]->messageSend("353 " + clients[clientFd]->getUserName() + " " + namesList);

    clients[clientFd]->messageSend("353 " + clients[clientFd]->getUserName() + " " + namesList);
    clients[clientFd]->messageSend("366 " + clients[clientFd]->getUserName() + " " + channelName + " :End of NAMES list\r\n");
}
