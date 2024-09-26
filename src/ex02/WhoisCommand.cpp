#include "WhoisCommand.hpp"
#include "Client.hpp"
#include <iostream>
#include <string>
#include <sstream>

void WhoisCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server &server) {
    (void)server;
    if (args.size() < 2) {
        clients[clientFd]->messageSend("461 WHOIS :Not enough parameters\r\n");
        return;
    }

    std::string target = args[1]; // Le nom d'utilisateur à interroger
    bool userFound = false;

    // Parcourir la liste des clients pour trouver l'utilisateur
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getUserName() == target) {
            // L'utilisateur a été trouvé, envoyer les informations
            std::ostringstream response;
            response << "311 " << target << " " << it->second->getUserName() << " "
                     << it->second->getHostName() << " * :User information\r\n";

            clients[clientFd]->messageSend(response.str());

            // Message générique pour les canaux, car ils ne sont pas encore gérés
            clients[clientFd]->messageSend("319 " + target + " :No channels listed\r\n");

            // Temps de connexion (en secondes d'inactivité)
            std::time_t currentTime = std::time(NULL);
            int secondsConnected = static_cast<int>(std::difftime(currentTime, it->second->getConnectTime()));

            std::ostringstream idleMessage;
            idleMessage << "317 " << target << " " << secondsConnected << " :seconds idle\r\n";
            clients[clientFd]->messageSend(idleMessage.str());

            // Fin de la commande WHOIS
            clients[clientFd]->messageSend("318 " + target + " :End of WHOIS list\r\n");

            userFound = true;
            break;
        }
    }

    // Si l'utilisateur n'a pas été trouvé
    if (!userFound) {
        clients[clientFd]->messageSend("401 " + target + " :No such nick\r\n");
    }
}


