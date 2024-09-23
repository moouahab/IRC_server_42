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



void Server::handleClient(int clientFd) {
    
    std::string message = _clients[clientFd]->getMessageClient();
    if (message.empty()) {
        closeClient(clientFd);
        return ;
    }

    

    std::vector<std::string> lines = splitString(message, '\n');
    CommandHandler commandHandler(_clients, _password);

    for (size_t i = 0; i < lines.size(); ++i) {

        std::string line = trim(lines[i]);
        
        std::vector<std::string> args = splitString(line, ' ');
        if (args.empty()) {
            _clients[clientFd]->messageSend("\033[31mInvalid command\r\n\033[0m");
            return ;
        }

        if (!_clients[clientFd]->getConnect() && lines.size() >= 2)
        {
            bool found = false;
            for (size_t j = 0; j < lines.size(); j++)
			{
                if (lines[j].substr(0, 4) == "PASS")
				{
                    found = true;
					break ;
				}
			}
            if (!found) {	
                _clients[clientFd]->messageSend("\033[31mMissing or incorrect PASS command\r\n\033[0m");
				closeClient(clientFd);
                return ;
            }
        }
		if (args[0] == "PRIVMSG")
        {
            std::string line = trim(lines[i]);
            args = splitString(line, ':');
            std::vector<std::string> private_keys = splitString(args[0], ' ');
            private_keys.push_back(args[1]);
            commandHandler.handleCommand(clientFd, private_keys);
        }
        else
            commandHandler.handleCommand(clientFd, args);
    }

    std::map<int, Client *>::iterator it = _clients.begin();
    for (;it != _clients.end(); it++)
    {
        std::cout << "[DEBUG] " << it->first << " " << it->second->getUserName() << std::endl;
    }
}

void Server::closeClient(int clientFd) {
    // Fermer le socket du client
    close(clientFd);
    // Supprimer le client de _sockFds
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


Client* Server::findClientByNickname(const std::string& nickname) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getUserName() == nickname) {
            return it->second;
        }
    }
    return NULL;
}

void Server::cleanupInactiveClients() {
    std::time_t now = std::time(NULL);
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ) {
        if (!it->second->getConnect() && (now - it->second->getConnectTime()) > 300) { // 5 minutes d'inactivité
            delete it->second;
            _clients.erase(it++);
        } else {
            ++it;
        }
    }
}