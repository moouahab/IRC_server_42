#include "OptionKey.hpp"

void OptionKey::apply(Channel* channel, Client* client, const std::string& argument) {
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    if (argument.empty()) {
        client->messageSend("461 MODE :Not enough parameters\r\n");
        return;
    }
    channel->setPassword(argument);
    channel->addMode('k');
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " +k " + argument + "\r\n";
    channel->broadcast(modeMessage, client);
}

void OptionKey::remove(Channel* channel, Client* client, const std::string& argument) {
    (void)argument;
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    channel->setPassword("");
    channel->removeMode('k');
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " -k\r\n";
    channel->broadcast(modeMessage, client);
}
