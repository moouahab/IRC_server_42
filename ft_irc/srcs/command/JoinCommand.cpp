#include "JoinCommand.hpp"
#include "Server.hpp"
#include <sstream>

void JoinCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    Client* client = clients[clientFd];
    // Vérifier le nombre d'arguments
    if (args.size() < 2) {
        client->messageSend("461 JOIN :Not enough parameters\r\n");
        Logger::log("Erreur : Nombre de paramètres insuffisant pour la commande JOIN du client " + client->getUserName());
        return;
    }
    std::string channelName = args[1];
    std::string key = "";
    if (args.size() >= 3)
        key = args[2];
    Logger::log("Le client " + client->getUserName() + " tente de rejoindre le canal " + channelName + " avec la clé '" + key + "'.");
    // Vérifier si le canal existe
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        // Créer le canal s'il n'existe pas
        Logger::log("Le canal " + channelName + " n'existe pas. Création du canal.");
        server.createChannel(channelName, client);
        channel = server.getChannel(channelName);
    }
    // Ajouter le client au canal
    if (!channel->addClient(client, key)) {
        Logger::log("Échec de l'ajout du client " + client->getUserName() + " au canal " + channelName + ".");
        return;
    }
    client->joinChannel(channel);
    Logger::log("Le client " + client->getUserName() + " a rejoint le canal " + channelName + ".");
    // Envoyer les messages appropriés
    std::string joinMessage = ":" + client->getPrefix() + " JOIN :" + channelName + "\r\n";
    channel->broadcast(joinMessage);
    Logger::log("Message JOIN diffusé sur le canal " + channelName + ".");
    // Envoyer le sujet du canal
    if (!channel->getTopic().empty()) {
        client->messageSend("332 " + client->getUserName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
        Logger::log("Sujet du canal " + channelName + " envoyé au client " + client->getUserName() + ".");
    } else {
        client->messageSend("331 " + client->getUserName() + " " + channelName + " :No topic is set\r\n");
        Logger::log("Aucun sujet défini pour le canal " + channelName + ".");
    }
    // Lister les utilisateurs du canal
    std::string namesList = "= " + channelName + " :";
    std::set<Client*> channelClients = channel->getClients();
    for (std::set<Client*>::iterator it = channelClients.begin(); it != channelClients.end(); ++it) {
        if (channel->isOperator(*it)) {
            namesList += "@" + (*it)->getUserName() + " ";
        } else {
            namesList += (*it)->getUserName() + " ";
        }
    }
    client->messageSend("353 " + client->getUserName() + " " + namesList + "\r\n");
    client->messageSend("366 " + client->getUserName() + " " + channelName + " :End of NAMES list\r\n");
    Logger::log("Liste des utilisateurs du canal " + channelName + " envoyée au client " + client->getUserName() + ".");
}
