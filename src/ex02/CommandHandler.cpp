#include "CommandHandler.hpp"
#include <string>
#include <sstream>
#include <iostream>

CommandHandler::CommandHandler(std::map<int, Client*>& clients, const std::string& password)
    : _clients(clients), _password(password) {}

CommandHandler::~CommandHandler() {}

void CommandHandler::handleCommand(int clientFd, const std::vector<std::string>& args) {
    if (args.empty()) return;

    std::string command = args[0];

    if (command == "PASS") {
        handlePass(clientFd, args);
    } else if (command == "PING") {
        handlePing(clientFd, args);
    } else if (command == "NICK") {
        handleNick(clientFd, args);
    } else if (command == "USER") {
        handleUser(clientFd, args);
    } else
        _clients[clientFd]->messageSend("\033[31mUnknown command\r\n\033[0m");

}

void CommandHandler::handlePass(int clientFd, const std::vector<std::string>& args) {
    if (!_clients[clientFd]->getConnect()) {
        if (args.size() >= 2 && args[1] == _password) {
            _clients[clientFd]->messageSend("\033[32mAuthentication successful\r\n\033[0m");
            _clients[clientFd]->setConnect(true);
        } else {
            _clients[clientFd]->messageSend("\033[31mAuthentication failed\r\n\033[0m");
        }
    }
}

void CommandHandler::handlePing(int clientFd, const std::vector<std::string>& args) {
    if (args.size() >= 2) {
        _clients[clientFd]->messageSend("PONG " + args[1] + "\r\n");
    } else {
        _clients[clientFd]->messageSend("421 PING :No origin specified\r\n");
    }
    _clients[clientFd]->messageSend("PONG\r\n");
}


void CommandHandler::handleNick(int clientFd, const std::vector<std::string>& args) {
    if (args.size() >= 2) {
        std::string newNick = args[1];

        // Vérification : le pseudonyme ne doit pas être vide
        if (newNick.empty()) {
            _clients[clientFd]->messageSend("431 NICK :No nickname given\r\n");
            return;
        }

        // Validation du format du pseudonyme (éviter les caractères spéciaux)
        for (size_t i = 0; i < newNick.length(); ++i) {
            if (!isalnum(newNick[i]) && newNick[i] != '_') {
                _clients[clientFd]->messageSend("432 " + newNick + " :Erroneous nickname\r\n");
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
            for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
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
        std::string oldNick = _clients[clientFd]->getUserName();

        // Si c'est la première fois que le client définit un pseudonyme, éviter d'envoyer oldNick
        if (oldNick.empty()) {
            _clients[clientFd]->messageSend("NICK " + finalNick + "\r\n");
        } else {
            _clients[clientFd]->messageSend(":" + oldNick + " NICK " + finalNick + "\r\n");
        }

        // Mettre à jour le pseudonyme du client
        std::cout << "\033[33m[INFO] new nickname : " << finalNick << "\033[0m"<< std::endl;
        _clients[clientFd]->setUserName(finalNick);

        // Notifier les autres clients du changement de pseudonyme
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
            if (it->first != clientFd) {
                if (oldNick.empty()) {
                    it->second->messageSend(":" + finalNick + " NICK " + finalNick + "\r\n");
                } else {
                    it->second->messageSend(":" + oldNick + " NICK " + finalNick + "\r\n");
                }
            }
        }

    } else {
        _clients[clientFd]->messageSend("431 NICK :No nickname given\r\n");
    }
}




// void CommandHandler::handleNick(int clientFd, const std::vector<std::string>& args) {
    
//     if (args.size() >= 2) {
//         std::string newNick = args[1];

//         // Vérification : le pseudonyme ne doit pas être vide
//         if (newNick.empty()) {
//             _clients[clientFd]->messageSend("431 NICK :No nickname given\r\n");
//             return;
//         }

//         // Validation du format du pseudonyme (éviter les caractères spéciaux)
//         for (size_t i = 0; i < newNick.length(); ++i) {
//             if (!isalnum(newNick[i]) && newNick[i] != '_') {
//                 _clients[clientFd]->messageSend("432 " + newNick + " :Erroneous nickname\r\n");
//                 return;
//             }
//         }

//         // Vérification si le pseudonyme est déjà utilisé
//         for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
//             if (it->second->getUserName() == newNick) {
//                 _clients[clientFd]->messageSend("433 " + newNick + " :Nickname is already in use\r\n");

//                 return ;
//             }
//         }

//         // Sauvegarder l'ancien pseudonyme
//         std::string oldNick = _clients[clientFd]->getUserName();

//         // Si c'est la première fois que le client définit un pseudonyme, éviter d'envoyer oldNick
//         if (oldNick.empty()) {
//             _clients[clientFd]->messageSend("NICK " + newNick + "\r\n");
//         } else {
//             _clients[clientFd]->messageSend(":" + oldNick + " NICK " + newNick + "\r\n");
//         }

//         // Mettre à jour le pseudonyme du client
//         std::cout << "new nickname : " << newNick << std::endl;
//         _clients[clientFd]->setUserName(newNick);

//         // Notifier les autres clients du changement de pseudonyme
//         for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
//             if (it->first != clientFd) {
//                 if (oldNick.empty()) {
//                     it->second->messageSend(":" + newNick + " NICK " + newNick + "\r\n");
//                 } else {
//                     it->second->messageSend(":" + oldNick + " NICK " + newNick + "\r\n");
//                 }
//             }
//         }

//     } else {
//         _clients[clientFd]->messageSend("431 NICK :No nickname given\r\n");
//     }
// }




void CommandHandler::handleUser(int clientFd, const std::vector<std::string>& args) {
    if (args.size() < 5) {
        _clients[clientFd]->messageSend("461 USER :Not enough parameters\r\n");
        return;
    }

    _clients[clientFd]->setHostName(args[1]);
    if (!_clients[clientFd]->getUserName().empty()) {
        _clients[clientFd]->messageSend("001 " + _clients[clientFd]->getUserName() + " :Welcome to the IRC server\r\n");
        _clients[clientFd]->messageSend("002 " + _clients[clientFd]->getUserName() + " :Your host is localhost\r\n");
        _clients[clientFd]->messageSend("003 " + _clients[clientFd]->getUserName() + " :This server was created for testing\r\n");
        _clients[clientFd]->messageSend("004 " + _clients[clientFd]->getUserName() + " localhost 1.0 i\r\n");
    }
}
