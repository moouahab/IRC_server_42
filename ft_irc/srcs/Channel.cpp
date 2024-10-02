#include "Channel.hpp"

Channel::Channel(const std::string& name, Client* creator, const std::string& password)
    : _name(name), _creator(creator), _password(password), _userLimit(0) {
    _clients.insert(creator);
    _operators.insert(creator);
}

Channel::~Channel() {}

bool Channel::addClient(Client* client, std::string key) {
    
    // Vérifier si le canal est en mode +k (protégé par mot de passe)
    if (hasMode('k') && getPassword() != key) {
        client->messageSend("475 " + client->getUserName() + " " + _name + " :Cannot join channel (+k)\r\n");
        Logger::log("Error  " + _name + " : " + client->getUserName() + " n'a pas ressuit a ce connecter (+k)");
        return false;
    }

    // Vérifier si le canal est en mode +i (invitation uniquement)
    if (hasMode('i') && !isInvited(client)) {
        client->messageSend("473 " + client->getUserName() + " " + _name + " :Cannot join channel (+i)\r\n");
        Logger::log("Error  " + _name + " : " + client->getUserName() + " n'a pas ressuit a ce connecter (+i)");
        return false;
    }

    // Vérifier si la limite d'utilisateurs est atteinte
    if (hasMode('l') && getUserLimit() > 0 && getClientCount() >= getUserLimit()) {
        client->messageSend("471 " + client->getUserName() + " " + _name + " :Cannot join channel (+l)\r\n");
        Logger::log("Error  " + _name + " : " + client->getUserName() + " n'a pas ressuit a ce connecter (+l)");
        return false;
    }
    _clients.insert(client);
    Logger::log( _name + " : " + client->getUserName() + " a ressuit a ce connecter");
    removeInvitation(client); // Supprimer l'invitation après que le client a rejoint
    return true;
}

void Channel::addOperator(Client* client) {
    if (isClientInChannel(client)) {
        _operators.insert(client);
        Logger::log( _name + " : " + client->getUserName() + " a ressuit a ce connecter entant Operator ");
    }
}

void Channel::broadcast(const std::string& message, Client* sender, bool topic) {
    if (sender != NULL)
        Logger::log( _name + " : " + sender->getUserName() + "a envoier un message.");
    if (topic || _operators.find(sender) != _operators.end()) 
    {
        for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
            (*it)->messageSend(message);
        }
    }
    else
    {
        for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
            if (*it != sender) {
                (*it)->messageSend(message);
            }
        }
    }

}

void Channel::removeMode(char mode) {
    size_t pos = _mode.find(mode);
    if (pos != std::string::npos) _mode.erase(pos, 1);
}

Client* Channel::getClientByName(const std::string& userName) const {
    for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getUserName() == userName) return *it;
    }
    return NULL;
}

void Channel::addMode(char mode) {
    if (_mode.find(mode) == std::string::npos) _mode += mode;
}

std::string         Channel::getName() const { return _name;}
std::string         Channel::getMode() const { return _mode; }
std::string         Channel::getTopic() const { return _topic; }
Client             *Channel::getCreator() const { return _creator; }
std::set<Client*>   Channel::getClients() const { return _clients; }
void                Channel::setMode(const std::string& mode) { _mode = mode; }
void                Channel::setTopic(const std::string& topic) { _topic = topic;}
void                Channel::removeClient(Client* client) { _clients.erase(client); }
bool                Channel::isPasswordProtected() const { return !_password.empty(); }
void                Channel::removeOperator(Client* client) { _operators.erase(client); }
bool                Channel::hasMode(char mode) const { return _mode.find(mode) != std::string::npos; }
bool                Channel::checkPassword(const std::string& password) const { return _password == password; }
bool                Channel::isOperator(Client* client) const { return _operators.find(client) != _operators.end();}
bool                Channel::isClientInChannel(Client* client) const { return _clients.find(client) != _clients.end(); }
