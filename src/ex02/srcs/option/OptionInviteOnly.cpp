#include "OptionInviteOnly.hpp"

void OptionInviteOnly::apply(Channel* channel, Client* client, const std::string& argument) {
    (void)argument;
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    channel->addMode('i');
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " +i\r\n";
    channel->broadcast(modeMessage);
}

void OptionInviteOnly::remove(Channel* channel, Client* client, const std::string& argument) {
    (void)argument;
    if (!channel->isOperator(client)) {
        client->messageSend("482 " + channel->getName() + " :You're not channel operator\r\n");
        return;
    }
    channel->removeMode('i');
    std::string modeMessage = ":" + client->getPrefix() + " MODE " + channel->getName() + " -i\r\n";
    channel->broadcast(modeMessage);
}
