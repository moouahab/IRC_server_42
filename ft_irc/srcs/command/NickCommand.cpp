#include "NickCommand.hpp"
#include <sstream>
#include <iostream>

void NickCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server& server) {
    (void) server;
    if (args.size() >= 2) {
        std::string newNick = args[1];

        // Vérification : pseudonyme vide ou avec caractères invalides
        if (newNick.empty()) {
            clients[clientFd]->messageSend("431 NICK :No nickname given\r\n");
            return ;
        }
        for (size_t i = 0; i < newNick.length(); ++i) {
            if (!isalnum(newNick[i]) && newNick[i] != '_') {
                clients[clientFd]->messageSend("432 " + newNick + " :Erroneous nickname\r\n");
                return ;
            }
        }

        // Générer un pseudonyme unique s'il est déjà pris
        std::string finalNick = newNick;
        bool nicknameTaken = false;
        int suffix = 1;

        do {
            nicknameTaken = false;
            for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second->getUserName() == finalNick) {
                    // Si le pseudonyme est pris, ajouter un suffixe avec le numéro du clientFd
                    std::ostringstream oss;
                    oss << newNick << "_" << suffix++;
                    finalNick = oss.str();
                    nicknameTaken = true;
                    break;
                }
            }
        } while (nicknameTaken);

        // Sauvegarder l'ancien pseudonyme
        std::string oldNick = clients[clientFd]->getUserName();
        if (oldNick.empty()) {
            clients[clientFd]->messageSend(":" + newNick + " NICK " + finalNick + "\r\n");
        } else {
            clients[clientFd]->messageSend(":" + oldNick + " NICK " + finalNick + "\r\n");
        }

        // Mettre à jour le pseudonyme du client
        clients[clientFd]->setUserName(finalNick);
        std::cout << "\033[33m[INFO] new nickname : " << finalNick << "\033[0m"<< std::endl;

        // Notifier les autres clients
        for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->first != clientFd) {
                it->second->messageSend(":" + (oldNick.empty() ? finalNick : oldNick) + " NICK " + finalNick + "\r\n");
            }
        }

        if (nicknameTaken) {
            clients[clientFd]->messageSend("Notice: Your nickname was taken. You have been assigned '" + finalNick + "'.\r\n");
        }

    } else {
        clients[clientFd]->messageSend("431 NICK :No nickname given\r\n");
    }
}

 