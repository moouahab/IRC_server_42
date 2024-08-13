#include "Serveur.hpp"

Serveur::Serveur(const std::string& pa, unsigned int p) : _port(p), _password(pa) {
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenFd < 0)
        throw std::runtime_error("Erreur : Impossible de créer le socket");
	
    int opt = 1;
    if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(_listenFd);
        throw std::runtime_error("Erreur : setsockopt(SO_REUSEADDR) a échoué");
	}

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_listenFd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		close(_listenFd);
        throw std::runtime_error("Erreur : Impossible de lier le socket à l'adresse");
	}
}

void Serveur::run() {
    if (listen(_listenFd, SOMAXCONN) < 0) {
        throw std::runtime_error("Erreur : Impossible de mettre le socket en écoute");
    }

    std::cout << "Serveur en écoute sur le port " << _port << std::endl;

    std::vector<pollfd> fds;

    // Ajouter le socket d'écoute à la liste des descripteurs surveillés
    pollfd listen_pollfd;
    listen_pollfd.fd = _listenFd;
    listen_pollfd.events = POLLIN;
    fds.push_back(listen_pollfd);

    while (true) {
        int poll_count = poll(fds.data(), fds.size(), -1);
        if (poll_count < 0) {
            throw std::runtime_error("Erreur : poll() a échoué");
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == _listenFd) {
                    accepterConnexion(fds);
                } else {
                    gérerClient(fds, i);
                }
            }
        }
    }
}

void Serveur::accepterConnexion(std::vector<pollfd>& fds) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(_listenFd, (struct sockaddr*)&client_addr, &client_len);
    
	if (client_fd < 0)
	{
        std::cerr << "Erreur : Impossible d'accepter la connexion" << std::endl;
        return;
    }
    std::cout << "Nouvelle connexion, fd: " << client_fd << std::endl;

    // Optionnel : Envoyer une demande de mot de passe au client
    std::string request_message = "Veuillez entrer votre mot de passe : ";
    send(client_fd, request_message.c_str(), request_message.size(), 0);
	
	// Lecture du mot de passe envoyé par le client
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received <= 0) {
        std::cerr << "Erreur : Impossible de lire les données du client" << std::endl;
        close(client_fd);
        return;
    }

    std::string received_password(buffer, bytes_received);
	std::cout << received_password << std::endl;
    if (received_password == _password) {
        std::cout << "Mot de passe correct, ajout du client" << std::endl;
        // Ajouter le client à la liste des descripteurs surveillés par poll()
        pollfd client_pollfd;
        client_pollfd.fd = client_fd;
        client_pollfd.events = POLLIN;
		
        fds.push_back(client_pollfd);
    } else {
        std::cerr << "Mot de passe incorrect, fermeture de la connexion" << std::endl;
        std::string error_message = "Mot de passe incorrect. Connexion fermée.\n";
        send(client_fd, error_message.c_str(), error_message.size(), 0);
        close(client_fd);
    }
}

void Serveur::gérerClient(std::vector<pollfd>& fds, size_t index) {
    int client_fd = fds[index].fd;

    // Lecture des données du client (par exemple, messages après connexion)
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received <= 0) {
        std::cerr << "Client déconnecté ou erreur, fd: " << client_fd << std::endl;
        close(client_fd);
        fds.erase(fds.begin() + index);
        return;
    }

    std::string client_message(buffer, bytes_received);
    std::cout << "Message reçu de fd " << client_fd << ": " << client_message << std::endl;

    // Traiter le message du client ici...
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

}

