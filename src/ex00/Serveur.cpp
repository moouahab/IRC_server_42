#include "Serveur.hpp"
#include <algorithm>
#include <string>


Serveur::Serveur(const std::string& password, unsigned int port)
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

void Serveur::run() {

    while (true) {
        int poll_count = poll(&_sockFds[0], _sockFds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Erreur lors de poll()" << std::endl;
            break;
        }

        for (size_t i = 0; i < _sockFds.size(); i++) {
            if (_sockFds[i].revents & POLLIN) {
                if (_sockFds[i].fd == _listenFd) {
                    accepterConnexion();
                } else {
                    gererClient(_sockFds[i].fd);
                }
            }
        }
    }
}

void Serveur::accepterConnexion() {
    struct sockaddr_in addr;
    socklen_t addrSize = sizeof(addr);
    int client_fd = accept(_listenFd, (sockaddr*)&addr, &addrSize);
    if (client_fd < 0) {
        std::cerr << "Erreur : Impossible d'accepter la connexion" << std::endl;
        return;
    }

    pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    _sockFds.push_back(pfd);

    char* client_ip = inet_ntoa(addr.sin_addr);
    t_client_info clientInfo = {client_fd, EN_ATTENTE_DE_MOT_DE_PASSE, 0, "", std::string(client_ip)};
    clients[client_fd] = clientInfo;

    std::string message = "Entrez votre mot de passe: ";
    send(client_fd, message.c_str(), message.size(), 0);

    std::cout << "Nouveau client connecté depuis " << client_ip << std::endl;
}

void Serveur::gererClient(int client_fd) {

    char buffer[256];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        std::cerr << "Erreur : Impossible de lire depuis le client ou connexion fermée" << std::endl;
        fermerClient(client_fd);
        return;
    }

    t_client_info &client = clients[client_fd];
    client.buffer += std::string(buffer);
    client.buffer.erase(std::remove(client.buffer.begin(), client.buffer.end(), '\n'), client.buffer.end());

    if (client.etat == EN_ATTENTE_DE_MOT_DE_PASSE) {
        if (client.buffer == _password) {
            client.etat = AUTHENTIFIÉ;
            std::cout << "Client authentifié depuis " << client.ip << std::endl;
            send(client_fd, "Mot de passe correct. Bienvenue !\n", 35, 0);
            client.buffer.clear();
        } else {
            client.tentatives++;
            if (client.tentatives >= 3) {
                send(client_fd, "Connexion fermée.", 19, 0);
                fermerClient(client_fd);
            } else {
                std::string message = "Mot de passe incorrect. Tentative\nEntrez votre mot de passe: ";
                send(client_fd, message.c_str(), message.size(), 0);
                client.buffer.clear();
            }
        }
    }

    if (client.etat == AUTHENTIFIÉ) {

        std::string message = client.buffer;
        client.buffer.clear();
        if (message == "/quit") {
            send(client_fd, "Vous avez quitté le serveur.\n", 29, 0);
            fermerClient(client_fd);
            return;
        }

        if (message.substr(0, 10) == "/broadcast") {
            std::string broadcastMessage = "[" + clients[client_fd].ip + "] " + message.substr(11); // Sauter "/broadcast "
            for (std::map<int, t_client_info>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second.etat == AUTHENTIFIÉ && it->first != client_fd) {
                    send(it->first, broadcastMessage.c_str(), broadcastMessage.size(), 0);
                }
            }
        } else {
            message += '\n';
            for (std::map<int, t_client_info>::iterator it = clients.begin(); it != clients.end(); ++it) {
                if (it->second.etat == AUTHENTIFIÉ && it->first != client_fd) {
                    send(it->first, message.c_str(), message.size(), 0);
                }
            }
        }
    }

}


void Serveur::fermerClient(int client_fd) {
    close(client_fd);
    for (std::vector<pollfd>::iterator it = _sockFds.begin(); it != _sockFds.end(); ++it) {
        if (it->fd == client_fd) {
            _sockFds.erase(it);
            break ;
        }
    }
    clients.erase(client_fd);
    std::cout << "Client déconnecté" << std::endl;
}


int  Serveur::getListenFd() const {
	return (this->_listenFd);
}

unsigned int Serveur::getPort() const {
	return (this->_port);
}

const std::string	&Serveur::getPassword() const {
	return (this->_password);
}

Serveur::~Serveur() {
    close(this->_listenFd);
}


