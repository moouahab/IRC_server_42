#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Logger.hpp"
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
    std::cout << "\033[35m"<< _userName << " Join channel " << channel->getName() << "\033[0m" << std::endl;
    _channels.insert(channel);
    Logger::log("Client " + _userName + " joined channel " + channel->getName());
}

void Client::leaveChannel(Channel* channel) {
    _channels.erase(channel);
    Logger::log("Client " + _userName + " left channel " + channel->getName());
}

std::set<Channel*> Client::getChannels() const {
    return _channels;
}

bool Client::isInChannel(const std::string& channelName) const {

    for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        std::cout <<"\033[35m NOM DES CHANEL ] [" << (*it)->getName() << "]\033[0m" << std::endl;
        if ((*it)->getName() == channelName) {
            std::cout << this->_userName << " is in " << channelName << std::endl;
            return true;
        }
    }
    return false;
}

std::string Client::getPrefix() const {
    return getUserName() + "!" + _userName + "@" + _hostName;
}


void Client::setJoinKey(const std::string& key) {
    _joinKey = key;
    Logger::log("Client " + _userName + " set join key to " + key);
}

std::string Client::getJoinKey() const {
    return _joinKey;
}

void Client::clearChannel(std::string channel) {
    Channel* channelToRemove = NULL;
    for (std::set<Channel*>::iterator it = _channels.begin(); it!= _channels.end(); ++it) {
        std::cout << "Removing " << (*it)->getName() << std::endl;
        if ((*it)->getName() == channel) {
            channelToRemove = *it;
            break;
        }
    }
    if (channelToRemove) {
        _channels.erase(channelToRemove);
        Logger::log("Client " + _userName + " left channel " + channel);
    }
}


void Client::clearJoinKey() {
    _joinKey.clear();
    Logger::log("Client " + _userName + " cleared join key.");
}

std::string Client::getChannelName() const {
    std::string nameChannel;
    for(std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        nameChannel += (*it)->getName() + " ";
        if(it!=_channels.end() && it!=--_channels.end())
        {
            nameChannel += ", ";
        }
    }
    return nameChannel;
}