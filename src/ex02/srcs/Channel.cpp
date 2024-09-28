#include "Channel.hpp"
#include "Client.hpp"
#include "Logger.hpp"
#include <iostream>
#include <set>

Channel::Channel(const std::string& name, Client* creator, const std::string& password)
    : _name(name), _creator(creator), _password(password), _userLimit(0) {
    _clients.insert(creator);
    _operators.insert(creator);
}

Channel::~Channel() {
    // Aucune ressource dynamique à libérer ici
}

bool Channel::hasMode(char mode) const {
    return _mode.find(mode) != std::string::npos;
}


bool Channel::addClient(Client* client) {
    // Vérifier la limite d'utilisateurs
    if (_userLimit > 0 && _clients.size() >= static_cast<size_t>(_userLimit)) {
        client->messageSend("471 " + _name + " :Cannot join channel (+l)\r\n");
        Logger::log("Channel " + _name + ": User limit reached. Cannot add client " + client->getUserName());
        return false;
    }

    // Vérifier le mode +i (invitation uniquement)
    if (hasMode('i') && !isInvited(client)) {
        client->messageSend("473 " + _name + " :Cannot join channel (+i)\r\n");
        Logger::log("Channel " + _name + ": Client " + client->getUserName() + " is not invited.");
        return false;
    }

    // Vérifier le mode +k (mot de passe)
    if (hasMode('k')) {
        std::string providedKey = client->getJoinKey(); // Assurez-vous que cette méthode existe et fonctionne correctement
        if (providedKey.empty() || _password != providedKey) {
            client->messageSend("475 " + _name + " :Cannot join channel (+k)\r\n");
            Logger::log("Channel " + _name + ": Incorrect key provided by client " + client->getUserName());
            return false;
        }
    }

    // Ajouter le client au canal
    _clients.insert(client);
    removeInvitation(client); // Supprimer l'invitation après avoir rejoint
    Logger::log("Channel " + _name + ": Client " + client->getUserName() + " added successfully.");
    return true;
}


void Channel::removeClient(Client* client) {
    // Vérifier si le client est dans le canal
    if (_clients.find(client) == _clients.end()) {
        Logger::log("Channel " + _name + ": Attempted to remove client " + client->getUserName() + " who is not in the channel.");
        return ; // Le client n'est pas dans le canal
    }

    // Retirer le client de la liste des clients
    _clients.erase(client);
    Logger::log("Channel " + _name + ": Client " + client->getUserName() + " removed from the channel.");

    // Si le client était un opérateur, le retirer de la liste des opérateurs
    if (_operators.find(client) != _operators.end()) {
        _operators.erase(client);
        Logger::log("Channel " + _name + ": Client " + client->getUserName() + " removed from operators.");
    }

    // Envoyer un message de notification aux autres membres du canal
    std::string quitMessage = ":" + client->getPrefix() + " QUIT :Client Quit\r\n";
    broadcast(quitMessage);

    // Si le canal devient vide, il peut être supprimé du serveur
    if (_clients.empty()) {
        Logger::log("Channel " + _name + " is now empty and will be deleted.");
        // Implémenter la suppression du canal dans le serveur si nécessaire
    }
}


void Channel::addOperator(Client* client) {
    if (isClientInChannel(client)) {
        _operators.insert(client);
    }
}

std::set<Client*> Channel::getClients() const { return _clients; }
bool Channel::isClientInChannel(Client* client) const { return _clients.find(client) != _clients.end(); }
void Channel::removeOperator(Client* client) { _operators.erase(client); }
bool Channel::isOperator(Client* client) const { return _operators.find(client) != _operators.end(); }
void Channel::setTopic(const std::string& topic) { _topic = topic; }
std::string Channel::getTopic() const { return _topic; }
void Channel::setMode(const std::string& mode) { _mode = mode; }
std::string Channel::getMode() const { return _mode;}
bool Channel::isPasswordProtected() const { return !_password.empty();}

bool Channel::checkPassword(const std::string& password) const { return _password == password; }

std::string Channel::getName() const { return _name; }

Client* Channel::getCreator() const { return _creator; }

void Channel::broadcast(const std::string& message, Client* sender) {
    for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        std::cout << (*it)->getUserName() << std::endl;
        if (*it != sender) {
            (*it)->messageSend(message);
        }
    }
}

Client* Channel::getClientByName(const std::string& userName) const {
    for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getUserName() == userName) {
            return *it;
        }
    }
    return NULL;
}

void Channel::addMode(char mode) {
    if (_mode.find(mode) == std::string::npos) {
        _mode += mode;
    }
}

void Channel::removeMode(char mode) {
    size_t pos = _mode.find(mode);
    if (pos != std::string::npos) {
        _mode.erase(pos, 1);
    }
}

