#include "Client.hpp"
#include <unistd.h> // pour close()
#include <sys/socket.h> // pour send() et recv()
#include <cstring>
#include <iostream>

Client::Client(int sockfd) : _sockfd(sockfd), _connected(false), _isIrssi(true) {
    _connectTime = std::time(NULL);
}

Client::~Client() {
	_connectTime = 0;
    close(_sockfd);
}

std::string Client::getMessageClient() {
	char buffer[1024];  // Taille du buffer pour lire les données
	std::memset(buffer, 0, sizeof(buffer));
	std::string fullMessage;  // Message complet à construire
	size_t bytesRead;
	size_t totalBytes = 0;  // Pour suivre le nombre total de bytes lus

	const size_t MAX_MESSAGE_SIZE = 65536;  // Taille maximale du message autorisé (par exemple 64 Ko)

	// Boucle pour s'assurer que l'on reçoit tous les paquets
	while ((bytesRead = recv(_sockfd, buffer, sizeof(buffer), 0)) > 0) {
		if (totalBytes + bytesRead > MAX_MESSAGE_SIZE) {
			std::cerr << "Message size exceeds maximum allowed size!" << std::endl;
			return "";  // On peut aussi fermer la connexion ici si c'est critique
		}
		// Vérification de la validité du contenu avant append (ici, on vérifie que le premier caractère est valide)
		if (std::isalnum(buffer[0])) {
			fullMessage.append(buffer, bytesRead);  // Ajouter les données lues au message complet
		} else {
			std::cerr << "Invalid data received!" << std::endl;
			return "";  // On peut aussi choisir d'ignorer ou de gérer cette erreur autrement
		}
		totalBytes += bytesRead;

		// Si la quantité de données reçues est inférieure à la taille du buffer,
		// cela signifie probablement que toutes les données ont été reçues
		if (bytesRead < sizeof(buffer)) {
			break;
		}
	}

	// Gérer les erreurs et déconnexions
	if (!(bytesRead > 0)) {
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
    ssize_t totalBytesSent = 0;
    ssize_t messageLength = message.length();
    const char* messageData = message.c_str();
    
    // Boucle pour s'assurer que tout le message est envoyé
    while (totalBytesSent < messageLength) {
        ssize_t bytesSent = send(_sockfd, messageData + totalBytesSent, messageLength - totalBytesSent, 0);
        
        if (bytesSent == -1) {
            std::cerr << "Error sending message to client " << _sockfd << std::endl;
            break ;
        }
        totalBytesSent += bytesSent;
    }
}

bool    Client:: isIrssiClientConnect() {
    static int i = 0;
    time_t end = time(NULL);

    if (i == 0 && difftime(end , getConnectTime()) > 5 && !getConnect()) 
    {
        i++;
        _isIrssi = false;
    }
	return _isIrssi;
}

bool Client::getConnect() const { return _connected; }
std::string Client::getUserName() const { return _userName; }
std::time_t Client::getConnectTime() const { return _connectTime; }
std::string Client::getHostName() const { return _hostName; }
int Client::getUserId() const { return _sockfd; }

bool Client::isIrssi() const { return _isIrssi; }

void Client::setConnect(bool value) { _connected = value; }
void Client::setUserName(const std::string &userName) { _userName = userName; }
void Client::setHostName(const std::string &hostName) { _hostName = hostName; }

bool Client::isSessionActive() {
    std::time_t now = std::time(NULL);
    double secondsElapsed = std::difftime(now, _connectTime);
    return secondsElapsed <= 60; // 300 secondes = 5 minutes
}