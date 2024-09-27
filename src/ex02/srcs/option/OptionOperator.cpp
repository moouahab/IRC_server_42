#include "OptionOperator.hpp"
#include "Channel.hpp"
#include "Client.hpp"

void OptionOperator::apply(Channel* channel, Client* client, const std::string& argument) {
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }

    Client* targetClient = channel->getClientByName(argument);
    if (!targetClient) {
        client->messageSend("401 " + argument + " :No such nick\r\n");
        return;
    }

    channel->addOperator(targetClient);
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " +o " + targetClient->getUserName() + "\r\n";
    channel->broadcast(modeMessage);
}

void OptionOperator::remove(Channel* channel, Client* client, const std::string& argument) {
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }

    Client* targetClient = channel->getClientByName(argument);
    if (!targetClient) {
        client->messageSend("401 " + argument + " :No such nick\r\n");
        return;
    }

    channel->removeOperator(targetClient);
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " -o " + targetClient->getUserName() + "\r\n";
    channel->broadcast(modeMessage);
}
