#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include "lib.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"

class Server {
private:
    unsigned int                    _port;
    int                             _listenFd;
    std::string                     _password;
    std::vector<pollfd>             _sockFds;
    std::map<int, Client *>         _clients;
    std::map<std::string, Channel*> _channels;
    std::string                     _serverName;
    CommandHandler*                 _commandHandler;

    void    acceptConnect();
    void    handleClient(int clientFd);

public:
    Server(const std::string& password, unsigned int port, std::string server);
    ~Server();
    void                run();
    std::string         getServerName() const;
    // Gestion des canaux
    Channel             *getChannel(const std::string& channelName);
    void                createChannel(const std::string& channelName, Client* creator, const std::string& password = "");
    void                removeChannel(const std::string& channelName);
    void                closeClient(int clientFd);
};

#endif
