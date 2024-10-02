#include "Client.hpp"
#include "Channel.hpp"
#include <unistd.h>
#include <sys/socket.h>
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
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    std::string fullMessage;
    ssize_t bytesRead = recv(_sockfd, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        _connected = false;
        return "";
    }

    fullMessage.append(buffer, bytesRead);
	if (bytesRead == 1 &&   fullMessage[0] == '\n')
        return " ";
	// std::cout <<"[DEBUG] message envoier " << fullMessage << std::endl;
	// std::cout << "[DEBUG] ";
	// for (size_t i = 0; i < fullMessage.size(); i++)
	// {
	// 	std::cout << static_cast<int>(fullMessage[i]);
	// }
	// std::cout << std::endl;
	
    return trim(fullMessage);
}

void Client::messageSend(const std::string &message) {
    send(_sockfd, message.c_str(), message.length(), 0);
}

bool Client::isIrssiClientConnect() {
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
    return secondsElapsed <= 60;
}

// Gestion des canaux
void Client::joinChannel(Channel* channel) {
    Logger::log("Le chanel join :" + channel->getName() + " avec sucess");
    _channels.insert(channel);
}

void Client::leaveChannel(Channel* channel) {
    _channels.erase(channel);
}

bool Client::isInChannel(const std::string& channelName) const {

    for (std::set<Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it) {
        std::cout << "Entering " << (*it)->getName() << " "  << this->getUserName()  << std::endl;
        if ((*it)->getName() == channelName) {
            return true;
        }
    }
    return false;
}

std::set<Channel*> Client::getChannels() const {
    return _channels;
}

std::string Client::getPrefix() const {
    return getUserName() + "!" + _userName + "@" + _hostName;
}
