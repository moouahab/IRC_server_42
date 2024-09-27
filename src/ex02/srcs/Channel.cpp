#include "Channel.hpp"

Channel::Channel(const std::string& name, Client* creator, const std::string& password)
    : _name(name), _creator(creator), _password(password), _userLimit(0) {
    _clients.insert(creator);
    _operators.insert(creator);
}

Channel::~Channel() {
    // Aucune ressource dynamique à libérer ici
}

// void Channel::addClient(Client* client) {
//     _clients.insert(client);
// }

bool Channel::hasMode(char mode) const {
    return _mode.find(mode) != std::string::npos;
}


bool Channel::addClient(Client* client) {
     if (hasMode('i') && !isInvited(client)) {
        client->messageSend("473 " + _name + " :Cannot join channel (+i)\r\n");
        return false;
    }

    if (_userLimit > 0 && _clients.size() >= static_cast<size_t>(_userLimit)) {
        client->messageSend("471 " + _name + " :Cannot join channel (+l)\r\n");
        return false;
    }
    _clients.insert(client);
    removeInvitation(client); // Supprimer l'invitation après que le client a rejoint
    return true;
}
void Channel::removeClient(Client* client) {
    _clients.erase(client);
}

bool Channel::isClientInChannel(Client* client) const {
    return _clients.find(client) != _clients.end();
}

std::set<Client*> Channel::getClients() const {
    return _clients;
}

void Channel::addOperator(Client* client) {
    if (isClientInChannel(client)) {
        _operators.insert(client);
    }
}

void Channel::removeOperator(Client* client) {
    _operators.erase(client);
}

bool Channel::isOperator(Client* client) const {
    return _operators.find(client) != _operators.end();
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

std::string Channel::getTopic() const {
    return _topic;
}

void Channel::setMode(const std::string& mode) {
    _mode = mode;
}

std::string Channel::getMode() const {
    return _mode;
}

bool Channel::isPasswordProtected() const {
    return !_password.empty();
}

bool Channel::checkPassword(const std::string& password) const {
    return _password == password;
}

std::string Channel::getName() const {
    return _name;
}

Client* Channel::getCreator() const {
    return _creator;
}

void Channel::broadcast(const std::string& message, Client* sender) {
    for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
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
