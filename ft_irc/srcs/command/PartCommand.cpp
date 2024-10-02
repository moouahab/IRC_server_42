// PartCommand.cpp
#include "PartCommand.hpp"
#include "Server.hpp"
#include <sstream>

void PartCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    if (args.size() < 2) {
        clients[clientFd]->messageSend("461 PART :Not enough parameters\r\n");
        return;
    }

    std::string channelsArg = args[1];
    std::string message = "See you all soon";

    // Si un message de départ est fourni
    if (args.size() > 2) {
        // Concaténer tous les arguments restants pour former le message
        for (size_t i = 2; i < args.size(); ++i) {
            if (i == 2 && args[i][0] == ':') {
                message = args[i].substr(1);
            } else {
                message += " " + args[i];
            }
        }
    }

    // Diviser les canaux s'il y en a plusieurs
    std::vector<std::string> channelNames = splitString(channelsArg, ',');

    for (size_t i = 0; i < channelNames.size(); ++i) {
        std::string channelName = channelNames[i];

        Channel* channel = server.getChannel(channelName);
        if (!channel) {
            clients[clientFd]->messageSend("403 " + channelName + " :No such channel\r\n");
            continue;
        }

        Client* client = clients[clientFd];
        if (!channel->isClientInChannel(client)) {
            clients[clientFd]->messageSend("442 " + channelName + " :You're not on that channel\r\n");
            continue;
        }

        // Construire le message de départ
        std::string partMessage = ":" + client->getUserName() + "!" + client->getUserName() + "@" + client->getHostName() +
                                  " PART " + channelName;
        if (!message.empty()) {
            partMessage += " :" + message;
        }
        partMessage += "\r\n";

        // Envoyer le message aux autres membres du canal
        channel->broadcast(partMessage, client, false);

        // Envoyer le message au client lui-même
        client->messageSend(partMessage);

        // Retirer le client du canal
        channel->removeClient(client);
        client->leaveChannel(channel);

        // Si le canal est vide, le supprimer
        if (channel->getClients().empty())
            server.removeChannel(channelName);
    }
}
