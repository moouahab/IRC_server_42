#include "WhoisCommand.hpp"
#include "Server.hpp"
#include <sstream>

void WhoisCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server &server) {
    if (args.size() < 2) {
        clients[clientFd]->messageSend("461 " + clients[clientFd]->getUserName() + " WHOIS :Not enough parameters\r\n");
        return;
    }

    std::string target = args[1]; // Le nom d'utilisateur à interroger
    bool userFound = false;

    // Parcourir la liste des clients pour trouver l'utilisateur
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getUserName() == target) {
            Client* targetClient = it->second;

            // RPL_WHOISUSER (311)
            std::ostringstream response;
            response << ":" << server.getServerName() << " 311 " << clients[clientFd]->getUserName() << " "
                     << targetClient->getUserName() << " " << targetClient->getHostName() << " * :"
                     << targetClient->getRealName() << "\r\n";
            clients[clientFd]->messageSend(response.str());

            // RPL_WHOISSERVER (312)
            std::ostringstream serverResponse;
            serverResponse << ":" << server.getServerName() << " 312 " << clients[clientFd]->getUserName() << " "
                           << targetClient->getUserName() << " " << server.getServerName() << " :Server Info\r\n";
            clients[clientFd]->messageSend(serverResponse.str());

            // RPL_WHOISCHANNELS (319)
            std::ostringstream channelList;
            channelList << ":" << server.getServerName() << " 319 " << clients[clientFd]->getUserName() << " "
                        << targetClient->getUserName() << " :";

            std::set<Channel*> userChannels = targetClient->getChannels();
            for (std::set<Channel*>::iterator chanIt = userChannels.begin(); chanIt != userChannels.end(); ++chanIt) {
                Channel* channel = *chanIt;
                if (channel->isOperator(targetClient)) {
                    channelList << "@" << channel->getName() << " ";
                } else {
                    channelList << channel->getName() << " ";
                }
            }
            channelList << "\r\n";
            clients[clientFd]->messageSend(channelList.str());
            // RPL_WHOISIDLE (317)
            std::time_t currentTime = std::time(NULL);
            int idleTime = static_cast<int>(std::difftime(currentTime, targetClient->getConnectTime()));
            std::ostringstream idleMessage;
            idleMessage << ":" << server.getServerName() << " 317 " << clients[clientFd]->getUserName() << " "
                        << targetClient->getUserName() << " " << idleTime << " :seconds idle\r\n";
            clients[clientFd]->messageSend(idleMessage.str());
            // RPL_ENDOFWHOIS (318)
            clients[clientFd]->messageSend(":" + server.getServerName() + " 318 " + clients[clientFd]->getUserName()
                                           + " " + targetClient->getUserName() + " :End of WHOIS list\r\n");

            userFound = true;
            break;
        }
    }

    // Si l'utilisateur n'a pas été trouvé
    if (!userFound) {
        clients[clientFd]->messageSend(":" + server.getServerName() + " 401 " + clients[clientFd]->getUserName() + " "
                                       + target + " :No such nick\r\n");
    }
}
