#include "Server.hpp"

Server::Server(const std::string& password, unsigned int port, std::string server)
    : _port(port), _password(password), _serverName(server){
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

    _commandHandler = new CommandHandler(*this, _clients, _password);
}

Server::~Server() {
    for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    delete _commandHandler;
    close(_listenFd);
}

void Server::run() {
    while (true) {
        int poll_count = poll(&_sockFds[0], _sockFds.size(), -1);

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

    for (size_t i = 0; i < lines.size(); ++i) {

        std::string line = trim(lines[i]);

        std::vector<std::string> args = splitString(line, ' ');
        if (args.empty()) {
            _clients[clientFd]->messageSend("Invalid command\r\n");
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
                _clients[clientFd]->messageSend("Missing or incorrect PASS command\r\n");
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
            _commandHandler->handleCommand(clientFd, private_keys);
        }
        else
            _commandHandler->handleCommand(clientFd, args);
    }
}

std::string Server::getServerName() const { return _serverName; }

void Server::closeClient(int clientFd) {
    Client  *client = _clients[clientFd];

    // Retirer le client de tous les canaux
    std::set<Channel*> clientChannels = client->getChannels();
    for (std::set<Channel*>::iterator it = clientChannels.begin(); it != clientChannels.end(); ++it) {
        (*it)->removeClient(client);
        std::string partMessage = ":" + client->getUserName() + " PART " + (*it)->getName() + "\r\n";
        (*it)->broadcast(partMessage);
        if ((*it)->getClients().empty()) {
            removeChannel((*it)->getName());
        }
    }

    // Supprimer le client
    close(clientFd);
    // Supprimer le client de _sockFds
    for (size_t i = 0; i < _sockFds.size(); ++i) {
        if (_sockFds[i].fd == clientFd) {
            _sockFds.erase(_sockFds.begin() + i);
            break;
        }
    }
    delete client;
    _clients.erase(clientFd);
    std::cout << "Client fermé : FD " << clientFd << std::endl;
}

Channel* Server::getChannel(const std::string& channelName) {
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        return it->second;
    }
    return NULL;
}

void Server::createChannel(const std::string& channelName, Client* creator, const std::string& password) {
    if (_channels.find(channelName) == _channels.end()) {
        Channel* newChannel = new Channel(channelName, creator, password);
        _channels[channelName] = newChannel;
    }
}

void Server::removeChannel(const std::string& channelName) {
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        delete it->second;
        _channels.erase(it);
    }
}
