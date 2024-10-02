#include "QuitCommand.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include <sstream>

void QuitCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    Client* client = clients[clientFd];
    std::string quitMessage = "Client Quit"; // Message par défaut

    if (args.size() > 1) {
        // Concaténer tous les arguments après QUIT pour former le message
        quitMessage = args[1];
        for (size_t i = 2; i < args.size(); ++i) {
            quitMessage += " " + args[i];
        }
        // Supprimer le ':' initial si présent
        if (!quitMessage.empty() && quitMessage[0] == ':') quitMessage = quitMessage.substr(1);
    }

    // Notifier tous les clients sur les mêmes canaux
    std::set<Channel*> clientChannels = client->getChannels();
    for (std::set<Channel*>::iterator it = clientChannels.begin(); it != clientChannels.end(); ++it) {
        Channel* channel = *it;
        // Construire le message de QUIT
        std::string quitMsg = ":" + client->getPrefix() + " QUIT :" + quitMessage + "\r\n";
        // Diffuser aux autres clients du canal
        channel->broadcast(quitMsg, client);
        // Retirer le client du canal
        channel->removeClient(client);
        client->leaveChannel(channel);
        // Si le canal est vide après le départ, le supprimer
        if (channel->getClients().empty()) {
            server.removeChannel(channel->getName());
        }
    }

    // Envoyer une notification au client
    client->messageSend("ERROR :Closing Link: " + client->getHostName() + " (" + quitMessage + ")\r\n");

    // Fermer la connexion du client
    server.closeClient(clientFd);
}
