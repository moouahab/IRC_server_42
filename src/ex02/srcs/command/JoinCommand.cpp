#include "JoinCommand.hpp"
#include "Server.hpp"
#include <sstream>

void JoinCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    Client* client = clients[clientFd];

    if (args.size() < 2) {
        client->messageSend("461 JOIN :Not enough parameters\r\n");
        return;
    }

    std::string channelName = args[1];
    std::string key = "";
    if (args.size() >= 3) {
        key = args[2];
    }

    // Vérifier si le canal existe
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        // Créer le canal s'il n'existe pas
        server.createChannel(channelName, client);
        channel = server.getChannel(channelName);
    }

    // Vérifier si le canal est en mode +k (protégé par mot de passe)
    if (channel->hasMode('k') && channel->getPassword() != key) {
        client->messageSend("475 " + channelName + " :Cannot join channel (+k)\r\n");
        return;
    }

    // Vérifier si le canal est en mode +i (invitation uniquement)
    if (channel->hasMode('i') && !channel->isInvited(client)) {
        client->messageSend("473 " + channelName + " :Cannot join channel (+i)\r\n");
        return;
    }

    // Vérifier si la limite d'utilisateurs est atteinte
    if (channel->hasMode('l') && channel->getUserLimit() > 0 && channel->getClientCount() >= channel->getUserLimit()) {
        client->messageSend("471 " + channelName + " :Cannot join channel (+l)\r\n");
        return;
    }

    // Ajouter le client au canal
    channel->addClient(client);
    client->joinChannel(channel);

    // Envoyer les messages appropriés
    std::string joinMessage = ":" + client->getPrefix() + " JOIN :" + channelName + "\r\n";
    channel->broadcast(joinMessage);

    // Envoyer le topic du canal
    if (!channel->getTopic().empty()) {
        client->messageSend("332 " + client->getUserName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
    } else {
        client->messageSend("331 " + client->getUserName() + " " + channelName + " :No topic is set\r\n");
    }

    // Lister les utilisateurs du canal
    std::string namesList = "= " + channelName + " :";
    std::set<Client*> channelClients = channel->getClients();
    for (std::set<Client*>::iterator it = channelClients.begin(); it != channelClients.end(); ++it) {
        if (channel->isOperator(*it)) {
            namesList += "@" + (*it)->getUserName() + " ";
        } else {
            namesList += (*it)->getUserName() + " ";
        }
    }
    client->messageSend("353 " + client->getUserName() + " " + namesList + "\r\n");
    client->messageSend("366 " + client->getUserName() + " " + channelName + " :End of NAMES list\r\n");
}
