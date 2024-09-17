/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moouahab <mohamed.ouahab1999@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 15:44:09 by moouahab          #+#    #+#             */
/*   Updated: 2024/09/14 19:59:53 by moouahab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP


#include <iostream>
#include "lib.hpp"
#include "Client.hpp"
#include "CommandHandler.hpp"


class Server {
private:
    unsigned int                _port;         // Port d'écoute du serveur
    int                         _listenFd;     // Socket d'écoute (file descriptor)
    std::string                 _password;     // Mot de passe pour se connecter

    // map pour gérer les clients (par leur pseudo ou leur socket)
    std::vector<pollfd>         _sockFds;      // Liste des sockets pour poll()
    std::map<int, Client *>     _clients;      // Associe les file descriptors (sockets) aux clients

    void acceptConnect();                    // Accepter une nouvelle connexion
    void handleClient(int clientFd);            // Gérer la communication avec un client existant
    void closeClient(int clientFd);             // Fermer la connexion d'un client

public:
    Server(const std::string& password, unsigned int port); // Constructeur
    ~Server() ; // Destructeur
    void                run();  // Lancer le serveur
};





#endif
