#include "OptionTopicRestriction.hpp"

void OptionTopicRestriction::apply(Channel* channel, Client* client, const std::string& argument) {
    (void)argument;
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    channel->addMode('t');
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " +t\r\n";
    channel->broadcast(modeMessage);
}

void OptionTopicRestriction::remove(Channel* channel, Client* client, const std::string& argument) {
    (void)argument;
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    channel->removeMode('t');
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " -t\r\n";
    channel->broadcast(modeMessage);
}
