#include "Channel.hpp"

Channel::Channel(const std::string& name, Client* creator, const std::string& password)
    : _name(name), _creator(creator), _password(password) {
    _clients.insert(creator);
    _operators.insert(creator);
}

Channel::~Channel() {
    // Aucune ressource dynamique à libérer ici
}

void Channel::addClient(Client* client) {
    _clients.insert(client);
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
