#include "JoinCommand.hpp"
#include "Server.hpp"
#include <sstream>
#include <algorithm>
#include <set> // pour std::transform

void JoinCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    Client* client = clients[clientFd];

    if (args.size() < 2) {
        client->messageSend("461 JOIN :Not enough parameters\r\n");
        Logger::log("JOIN command failed: Not enough parameters for user " + client->getUserName());
        return;
    }

    std::string channelName = args[1];
    std::string key = "";
    if (args.size() >= 3) {
        key = args[2];
        client->setJoinKey(key); // Stocker temporairement le mot de passe
    }

    Logger::log("User " + client->getUserName() + " attempting to join channel: " + channelName + " with key: " + key);

    // Normaliser le nom du canal en minuscules
    std::transform(channelName.begin(), channelName.end(), channelName.begin(), ::tolower);

    // Vérifier si le canal existe
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        Logger::log("Channel " + channelName + " does not exist. Creating channel.");
        server.createChannel(channelName, client, key);
        channel = server.getChannel(channelName);
    } else {
        // Check if the channel is invite-only and the client is not invited
        if (channel->hasMode('i') && !channel->isInvited(client)) {
            client->messageSend("473 " + channelName + " :Cannot join channel (+i)\r\n");
            Logger::log("Failed to join channel " + channelName + ": User " + client->getUserName() + " is not invited");
            client->clearJoinKey();
        }

        // Vérifier si le canal est en mode +k (protégé par mot de passe)
        if (channel->hasMode('k') && !channel->checkPassword(key)) {
            client->messageSend("475 " + channelName + " :Cannot join channel (+k)\r\n");
            Logger::log("Failed to join channel " + channelName + ": Incorrect key provided by user " + client->getUserName());
            client->clearJoinKey();  // Nettoyer le mot de passe temporaire
        }

        // Vérifier si la limite d'utilisateurs est atteinte
        if (channel->hasMode('l') && channel->getUserLimit() > 0 && channel->getClientCount() >= channel->getUserLimit()) {
            client->messageSend("471 " + channelName + " :Cannot join channel (+l)\r\n");
            Logger::log("Failed to join channel " + channelName + ": User limit reached for user " + client->getUserName());
            client->clearJoinKey();  // Nettoyer le mot de passe temporaire
        }
    }



    // Ajouter le client au canal et vérifier le succès
    bool added = channel->addClient(client);
    if (!added) {
        // Les messages d'erreur sont déjà envoyés dans addClient
        Logger::log("Failed to add client " + client->getUserName() + " to channel " + channelName);
        channel->removeClient(client);
        client->leaveChannel(channel);
        client->clearJoinKey();
        return ;
    }
    // Ajouter le canal à la liste des canaux du client
    client->joinChannel(channel);
    Logger::log("Client " + client->getUserName() + " successfully joined channel " + channelName);

    // Envoyer les messages appropriés
    std::string joinMessage = ":" + client->getPrefix() + " JOIN :" + channelName + "\r\n";
    channel->broadcast(joinMessage, client);
    Logger::log("Broadcasted JOIN message to channel " + channelName);

    // Envoyer le topic du canal
    if (!channel->getTopic().empty()) {
        client->messageSend("332 " + client->getUserName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
        Logger::log("Sent topic to client " + client->getUserName() + " for channel " + channelName);
    } else {
        client->messageSend("331 " + client->getUserName() + " " + channelName + " :No topic is set\r\n");
        Logger::log("No topic set for channel " + channelName + ". Sent message to client " + client->getUserName());
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
    Logger::log("Sent NAMES list to client " + client->getUserName() + " for channel " + channelName);
    client->messageSend("366 " + client->getUserName() + " " + channelName + " :End of NAMES list\r\n");
    Logger::log("Sent End of NAMES list to client " + client->getUserName() + " for channel " + channelName);
}
