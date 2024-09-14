#include "Server.hpp"

Server::Server(const std::string& password, unsigned int port)
    : _port(port), _password(password) {
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (_listenFd < 0) {
        throw std::runtime_error("Erreur : Impossible de créer le socket");
    }
    
    if (fcntl(_listenFd, F_SETFL, O_NONBLOCK) < 0) {
        close(_listenFd);
        throw std::runtime_error("Erreur : Impossible de configurer le socket en mode non-bloquant");
    }
    
    int opt = 1;
    if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(_listenFd);
        throw std::runtime_error("Erreur : setsockopt(SO_REUSEADDR) a échoué");
    }
    
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_listenFd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(_listenFd);
        throw std::runtime_error("Erreur : Impossible de lier le socket à l'adresse");
    }
    
    if (listen(_listenFd, SOMAXCONN) < 0) {
        close(_listenFd);
        throw std::runtime_error("Erreur : Impossible de mettre le socket en écoute");
    }

    pollfd pfd;
    pfd.fd = _listenFd;
    pfd.events = POLLIN;
    _sockFds.push_back(pfd);
}

void Server::run() {

    while (true) {
        int poll_count = poll(&this->_sockFds[0], _sockFds.size(), -1);

        if (poll_count < 0) {
            std::cerr << "Erreur lors de poll()" << std::endl;
            break ;
        }

        for (size_t i = 0; i < _sockFds.size(); i++)
        {
            if (_sockFds[i].revents & POLLIN) {
                if (_sockFds[i].fd == this->_listenFd)
                    acceptConnect();
                else
                {
                    handleClient(_sockFds[i].fd);
                }
            }
        }
    }
}


void Server::acceptConnect() {
    struct sockaddr_in addr;
    socklen_t addrSize = sizeof(addr);
    int client_fd = accept(_listenFd, (sockaddr*)&addr, &addrSize);
    if (client_fd < 0) {
        std::cerr << "Erreur : Impossible d'accepter la connexion" << std::endl;
        return;
    }

    // Créer un nouveau client et l'ajouter à la map
    Client *newClient = new Client(client_fd);
    _clients[client_fd] = newClient;

    pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _sockFds.push_back(pfd);
    std::cout << "Nouveau client connecté : FD " << client_fd << std::endl;
}

void Server::handleClient(int clientFd)
{
    // Récupérer le message du client
    std::string message = _clients[clientFd]->getMessageClient();
    if (message.empty()) {
        closeClient(clientFd);
        return;
    }

    // Diviser le message en plusieurs lignes s'il y en a
    std::vector<std::string> lines = splitString(message, '\n');
    for (size_t j = 0; j < lines.size(); ++j) {

        std::string line = trim(lines[j]);  // Nettoyer chaque ligne
        std::vector<std::string> args = splitString(line, ' ');// Diviser la ligne en arguments


        // Validation de la commande
        if (args.size() < 2) {
            _clients[clientFd]->messageSend("\033[31mInvalid command\r\n\033[0m");
            continue ;
        }

        if (!_clients[clientFd]->getConnect())
        {
            bool found = false;
            for (size_t i = 0; i < lines.size(); ++i)
                if (lines[i].substr(0, 4) == "PASS")
                    found = true;
            if (!found) {
                _clients[clientFd]->messageSend("\033[31mMissing or incorrect PASS command\r\n\033[0m");
                closeClient(clientFd);
                return ;
            }
        }

        // Gestion de la commande PASS
        if (args[0] == "PASS" && !_clients[clientFd]->getConnect()) {
            if (args[1] == _password) {
                _clients[clientFd]->messageSend("\033[32mAuthentication successful\r\n\033[0m");
                std::cout << "Authentication successful for " << clientFd << std::endl;
                _clients[clientFd]->setConnect(true);
            } else {
                _clients[clientFd]->messageSend("\033[31mAuthentication failed with error code\r\n\033[0m");
                closeClient(clientFd);
                return 
                ;
            }
        }

        if (_clients[clientFd]->getConnect()) {
            
            if (args.size() == 2 && args[0] == "PING") {
                _clients[clientFd]->messageSend("PONG\r\n");
                std::cout << "PONG sent to client " << clientFd << std::endl;
                continue ;
            }
            
            // Gestion de la commande NICK
            if (args[0] == "NICK") {

                _clients[clientFd]->setUserName(args[1]);
                _clients[clientFd]->messageSend("NICK " + args[1] + "\r\n");
                std::cout << "Le client " << clientFd << " a défini son nom d'utilisateur : " << args[1] << std::endl;
            }
            
            // Gestion de la commande USER
            if (args[0] == "USER") {
                // Vérifier que la commande USER a au moins 4 paramètres
                if (args.size() < 5) {
                    _clients[clientFd]->messageSend("461 USER :Not enough parameters\r\n");
                    return ;
                }

                _clients[clientFd]->setHostName(args[1]);  // Définir le nom réel de l'utilisateur
                std::cout << "Le client " << clientFd << " a défini son nom réel : " << args[1] << std::endl;

                // Une fois que `NICK` et `USER` sont définis, Irssi attend un message de bienvenue
                if (!_clients[clientFd]->getUserName().empty()) {
                    _clients[clientFd]->messageSend("001 " + _clients[clientFd]->getUserName() + " :Welcome to the IRC server\r\n");
                    _clients[clientFd]->messageSend("002 " + _clients[clientFd]->getUserName() + " :Your host is localhost\r\n");
                    _clients[clientFd]->messageSend("003 " + _clients[clientFd]->getUserName() + " :This server was created for testing\r\n");
                    _clients[clientFd]->messageSend("004 " + _clients[clientFd]->getUserName() + " localhost 1.0 i\r\n");
                }
            }
            
            if (_clients[clientFd]->getConnect())
                std::cout << "Message du client " << clientFd << ": " << line << std::endl;
        }
    }
}



void Server::closeClient(int clientFd) {
    // Fermer le socket du client
    close(clientFd);
    delete _clients[clientFd];
    _clients.erase(clientFd);
    for (size_t i = 0; i < _sockFds.size(); ++i) {
        if (_sockFds[i].fd == clientFd) {
            _sockFds.erase(_sockFds.begin() + i);
            break;
        }
    }
    std::cout << "Client fermé : FD " << clientFd << std::endl;
}

Server::~Server() {

    std::map<int, Client *>::iterator it = _clients.begin();
    for (; it != _clients.end(); ++it) {
        delete it->second;
    }
    close(_listenFd);
}