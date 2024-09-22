#include "NickCommand.hpp"
#include <sstream>
#include <iostream>

void NickCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args) {
    if (args.size() >= 2) {
        std::string newNick = args[1];

        // Vérification : le pseudonyme ne doit pas être vide
        if (newNick.empty()) {
            clients[clientFd]->messageSend("431 NICK :No nickname given\r\n");
            return;
        }

        // Validation du format du pseudonyme (éviter les caractères spéciaux)
        for (size_t i = 0; i < newNick.length(); ++i) {
            if (!isalnum(newNick[i]) && newNick[i] != '_') {
                clients[clientFd]->messageSend("432 " + newNick + " :Erroneous nickname\r\n");
                return;
            }
        }

        // Générer un nouveau pseudonyme si le premier est déjà pris
        std::string finalNick = newNick;
        int suffix = 1;
        bool nicknameTaken = true;

        // Boucle jusqu'à trouver un pseudonyme disponible
        while (nicknameTaken) {
            nicknameTaken = false;

            // Vérification si le pseudonyme est déjà utilisé
            for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second->getUserName() == finalNick) {
                    // Si le pseudonyme est pris, ajouter un suffixe avec ostringstream
                    std::ostringstream oss;
                    oss << newNick << suffix++;
                    finalNick = oss.str();
                    nicknameTaken = true;
                    break;
                }
            }
        }

        // Sauvegarder l'ancien pseudonyme
        std::string oldNick = clients[clientFd]->getUserName();
        // Si c'est la première fois que le client définit un pseudonyme, éviter d'envoyer oldNick
        if (oldNick.empty()) {
            clients[clientFd]->messageSend("NICK " + finalNick + "\r\n");
        } else {
            clients[clientFd]->messageSend(oldNick + " NICK " + finalNick + "\r\n");
        }

        // Mettre à jour le pseudonyme du client
        std::cout << "\033[33m[INFO] new nickname : " << finalNick << "\033[0m"<< std::endl;
        clients[clientFd]->setUserName(finalNick);

        // Notifier les autres clients du changement de pseudonyme
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->first != clientFd) {
                if (oldNick.empty()) {
                    it->second->messageSend(finalNick + " NICK " + finalNick + "\r\n");
                } else {
                    it->second->messageSend(oldNick + " NICK " + finalNick + "\r\n");
                }
            }
        }

    } else {
        clients[clientFd]->messageSend("431 NICK :No nickname given\r\n");
    }
}
 