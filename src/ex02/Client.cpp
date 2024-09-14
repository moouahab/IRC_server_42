#include "Client.hpp"
#include <unistd.h> // pour close()
#include <sys/socket.h> // pour send() et recv()
#include <cstring>
#include <iostream>

Client::Client(int sockfd) : _sockfd(sockfd), _connected(false) {
    _connectTime = std::time(NULL);
}

Client::~Client() {
    close(_sockfd);
}


std::string   Client::getMessageClient() {
    char buffer[1024];  // Taille du buffer pour lire les données
    std::string fullMessage;  // Message complet à construire
    size_t bytesRead;

    // Boucle pour s'assurer que l'on reçoit tous les paquets
    while ((bytesRead = recv(_sockfd, buffer, sizeof(buffer), 0)) > 0) {
        fullMessage.append(buffer, bytesRead);  // Ajouter les données lues au message complet

        // Si la quantité de données reçues est inférieure à la taille du buffer,
        // cela signifie probablement que toutes les données ont été reçues
        if (bytesRead < sizeof(buffer)) {
            break ;
        }
    }
    
    // Gérer les erreurs et déconnexions
    if (!(bytesRead > 0)) {
        std::cerr << "Error receiving message from client: " << _sockfd << std::endl;
        _connected = false;
        return "";
    } else if (bytesRead == 0) {
        std::cerr << "Client disconnected: FD " << _sockfd << std::endl;
        _connected = false;
        return "";
    }
    
    // Nettoyer les retours à la ligne du message reçu
    return trim(fullMessage);
}


void Client::messageSend(const std::string &message) {
    ssize_t bytesSent = send(_sockfd, message.c_str(), message.length(), 0);
    
    if (bytesSent == -1)
        std::cerr << "Error sending message to client " << _sockfd << std::endl;
}

bool Client::getConnect() const { return _connected; }
std::string Client::getUserName() const { return _userName; }
std::time_t Client::getConnectTime() const { return _connectTime; }
std::string Client::getHostName() const { return _hostName; }

void Client::setConnect(bool value) { _connected = value; }
void Client::setUserName(const std::string &userName) { _userName = userName; }
void Client::setHostName(const std::string &hostName) { _hostName = hostName; }

