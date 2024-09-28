#include "TopicCommand.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <sstream>
#include <iostream>

void TopicCommand::execute(int clientFd, std::map<int, Client*>& clients,
                           const std::vector<std::string>& args, Server& server) {
    Client* client = clients[clientFd];

    if (args.size() < 2) {
        client->messageSend("461 TOPIC :Not enough parameters\r\n");
        return;
    }

    std::string channelName = args[1];
    std::string newTopic = "";

    if (args.size() >= 3) {
        // Le sujet peut commencer par ':' indiquant que le reste est le sujet
        newTopic = args[2];
        for (size_t i = 3; i < args.size(); ++i) {
            newTopic += " " + args[i];
        }
        if (!newTopic.empty() && newTopic[0] == ':') {
            newTopic = newTopic.substr(1);
        }
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        client->messageSend("403 " + channelName + " :No such channel\r\n");
        return;
    }

    if (!channel->isClientInChannel(client)) {
        client->messageSend("442 " + channelName + " :You're not on that channel\r\n");
        return;
    }

    if (args.size() >= 3) {
        // Définir le sujet
        if (channel->hasMode('t') && !channel->isOperator(client)) {
            client->messageSend("482 " + channelName + " :You're not channel operator\r\n");
            return;
        }
        channel->setTopic(newTopic);
        std::string topicMessage = ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        channel->broadcast(topicMessage, client);
    } else {
        // Obtenir le sujet
        if (!channel->getTopic().empty()) {
            std::stringstream ss;
            ss << "332 " << client->getUserName() << " " << channelName << " :" << channel->getTopic() << "\r\n";
            client->messageSend(ss.str());
        } else {
            std::stringstream ss;
            ss << "331 " << client->getUserName() << " " << channelName << " :No topic is set\r\n";
            client->messageSend(ss.str());
        }
    }
}
