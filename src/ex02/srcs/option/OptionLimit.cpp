#include "OptionLimit.hpp"
#include <cstdlib> // pour std::atoi

void OptionLimit::apply(Channel* channel, Client* client, const std::string& argument) {
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    if (argument.empty()) {
        client->messageSend("461 MODE :Not enough parameters\r\n");
        return;
    }
    int limit = std::atoi(argument.c_str());
    if (limit <= 0) {
        client->messageSend("461 MODE :Invalid limit\r\n");
        return;
    }
    channel->setUserLimit(limit);
    channel->addMode('l');
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " +l " + argument + "\r\n";
    channel->broadcast(modeMessage, client);
}

void OptionLimit::remove(Channel* channel, Client* client, const std::string& argument) {
    (void)argument;
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    channel->setUserLimit(0);
    channel->removeMode('l');
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " -l\r\n";
    channel->broadcast(modeMessage, client);
}
