#include "ModeCommand.hpp"
#include "Server.hpp"

void ModeCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    if (args.size() < 3) {
        clients[clientFd]->messageSend("461 MODE :Not enough parameters\r\n");
        return;
    }

    std::string channelName = args[1];
    std::string modeChanges = args[2];
    std::string targetUser;

    if (args.size() > 3) {
        targetUser = args[3];
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        clients[clientFd]->messageSend("403 " + channelName + " :No such channel\r\n");
        return;
    }

    Client* client = clients[clientFd];

    if (!channel->isClientInChannel(client)) {
        clients[clientFd]->messageSend("442 " + channelName + " :You're not on that channel\r\n");
        return;
    }

    if (!channel->isOperator(client)) {
        clients[clientFd]->messageSend("482 " + channelName + " :You're not channel operator\r\n");
        return;
    }

    bool adding = false;
    for (size_t i = 0; i < modeChanges.size(); ++i) {
        char modeChar = modeChanges[i];
        if (modeChar == '+') {
            adding = true;
        } else if (modeChar == '-') {
            adding = false;
        } else if (modeChar == 'o') {
            if (targetUser.empty()) {
                clients[clientFd]->messageSend("461 MODE :Not enough parameters\r\n");
                return;
            }

            // Trouver le client cible
            Client* targetClient = NULL;
            for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second->getUserName() == targetUser) {
                    targetClient = it->second;
                    break;
                }
            }

            if (!targetClient) {
                clients[clientFd]->messageSend("401 " + targetUser + " :No such nick\r\n");
                return;
            }

            if (!channel->isClientInChannel(targetClient)) {
                clients[clientFd]->messageSend("441 " + targetUser + " " + channelName + " :They aren't on that channel\r\n");
                return;
            }

            if (adding) {
                channel->addOperator(targetClient);
                std::string modeMessage = ":" + client->getUserName() + " MODE " + channelName + " +o " + targetUser + "\r\n";
                channel->broadcast(modeMessage);
            } else {
                channel->removeOperator(targetClient);
                std::string modeMessage = ":" + client->getUserName() + " MODE " + channelName + " -o " + targetUser + "\r\n";
                channel->broadcast(modeMessage);
            }
        }
        // Gérer d'autres modes si nécessaire
    }
}
