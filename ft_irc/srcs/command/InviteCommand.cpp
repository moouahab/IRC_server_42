#include "InviteCommand.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <sstream>
#include <iostream>

void InviteCommand::execute(int clientFd, std::map<int, Client*>& clients,
                            const std::vector<std::string>& args, Server& server) {
    Client* client = clients[clientFd];
    
    if (args.size() < 3) {
        client->messageSend("461 INVITE :Not enough parameters\r\n");
        return;
    }
    
    std::string targetNick = args[1];
    std::string channelName = args[2];
    
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        client->messageSend("403 " + channelName + " :No such channel\r\n");
        return;
    }
    
    if (!channel->isClientInChannel(client)) {
        client->messageSend("442 " + channelName + " :You're not on that channel\r\n");
        return ;
    }
    
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channelName + " :You're not channel operator\r\n");
        return;
    }
    
    // Trouver le client cible
    Client* targetClient = NULL;
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getUserName() == targetNick) {
            targetClient = it->second;
            break;
        }
    }
    
    if (!targetClient) {
        client->messageSend("401 " + targetNick + " :No such nick/channel\r\n");
        return;
    }
    
    // Ajouter le client cible aux invitations du canal
    channel->inviteClient(targetClient);
    
    // Envoyer le message d'invitation au client cible
    std::string inviteMessage = ":" + client->getPrefix() + " INVITE " + targetNick + " " + channelName + "\r\n";
    targetClient->messageSend(inviteMessage);
    
    // Notifier le client qui a invité que l'invitation a été envoyée
    std::stringstream ss;
    ss << "341 " << client->getUserName() << " " << targetNick << " " << channelName << " :You have been invited to the channel\r\n";
    client->messageSend(ss.str());
}
