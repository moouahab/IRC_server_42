#include "WhoisCommand.hpp"
#include "Client.hpp"
#include <iostream>
#include <sstream>

void WhoisCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args) {
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
            clients[clientFd]->messageSend("319 " + target + " :No channels listed\r\n");
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
