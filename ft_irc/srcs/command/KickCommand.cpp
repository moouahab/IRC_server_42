#include "KickCommand.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <sstream>
#include <iostream>

void KickCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    Client* client = clients[clientFd];
    
    if (args.size() < 3) {
        client->messageSend("461 KICK :Not enough parameters\r\n");
        return;
    }
    
    std::string channelName = args[1];
    std::string targetNick = args[2];
    std::string comment = "Kicked by operator";
    
    if (args.size() >= 4) {
        // Le commentaire peut commencer par ':' indiquant que le reste est le commentaire
        comment = args[3];
        for (size_t i = 4; i < args.size(); ++i) {
            comment += " " + args[i];
        }
        if (!comment.empty() && comment[0] == ':') {
            comment = comment.substr(1);
        }
    }
    
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        client->messageSend("403 " + channelName + " :No such channel\r\n");
        return ;
    }
    
    if (!channel->isClientInChannel(client)) {
        client->messageSend("442 " + channelName + " :You're not on that channel\r\n");
        return ;
    }
    
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channelName + " :You're not channel operator\r\n");
        return ;
    }
    
    Client* targetClient = channel->getClientByName(targetNick);
    if (!targetClient) {
        client->messageSend("441 " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
        return ;
    }
    
    // Construire le message de KICK
    std::string kickMessage = ":" + client->getPrefix() + " KICK " + channelName + " " + targetNick + " :" + comment + "\r\n";
    
    // Envoyer le message de KICK aux autres membres du canal
    channel->broadcast(kickMessage, targetClient); // Envoyer à tous sauf le cible
    targetClient->messageSend(kickMessage); // Envoyer au cible également
    
    // Retirer le client du canal
    channel->removeClient(targetClient);
    targetClient->leaveChannel(channel);
    
    // Si le canal est vide, le supprimer
    if (channel->getClients().empty())
        server.removeChannel(channelName);
}
