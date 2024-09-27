#include "WhoCommand.hpp"
#include "Server.hpp"
#include "Channel.hpp"

void WhoCommand::execute(int clientFd, std::map<int, Client*>& clients,
                         const std::vector<std::string>& args, Server& server) {
    Client* requestingClient = clients[clientFd];
    std::string mask = "*"; // Par défaut, le masque est "*"

    if (args.size() > 1) {
        mask = args[1];
    }

    bool found = false;

    // Si le masque est un canal
    if (mask[0] == '#') {
        Channel* channel = server.getChannel(mask);
        if (channel) {
            std::set<Client*> channelClients = channel->getClients();
            for (std::set<Client*>::iterator it = channelClients.begin(); it != channelClients.end(); ++it) {
                Client* client = *it;
                // Construire la réponse RPL_WHOREPLY (352)
                std::string response = ":" + server.getServerName() + " 352 " + requestingClient->getUserName() + " ";
                response += channel->getName() + " ";
                response += client->getUserName() + " ";
                response += client->getHostName() + " ";
                response += server.getServerName() + " ";
                response += client->getUserName() + " ";
                response += "H :0 "; // "H" pour "Here", "G" pour "Gone" (AWAY)
                response += client->getRealName() + "\r\n";
                requestingClient->messageSend(response);
                found = true;
            }
        } else {
            // Le canal n'existe pas
            requestingClient->messageSend("403 " + mask + " :No such channel\r\n");
            return;
        }
    } else {
        // Parcourir tous les clients
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            Client* client = it->second;
            // Vérifier si le pseudonyme correspond au masque
            if (matchWildcard(mask, client->getUserName())) {
                // Construire la réponse RPL_WHOREPLY (352)
                std::string response = ":" + server.getServerName() + " 352 " + requestingClient->getUserName() + " ";
                response += "* "; // Pas de canal spécifique
                response += client->getUserName() + " ";
                response += client->getHostName() + " ";
                response += server.getServerName() + " ";
                response += client->getUserName() + " ";
                response += "H :0 ";
                response += client->getRealName() + "\r\n";
                requestingClient->messageSend(response);
                found = true;
            }
        }
    }

    // Si aucun utilisateur n'a été trouvé
    if (!found) {
        requestingClient->messageSend("401 " + mask + " :No such nick/channel\r\n");
    }

    // Envoyer RPL_ENDOFWHO
    std::string endResponse = ":" + server.getServerName() + " 315 " + requestingClient->getUserName() + " " + mask + " :End of WHO list\r\n";
    requestingClient->messageSend(endResponse);
}
