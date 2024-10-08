#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Client.hpp"
#include <vector>
#include <string>

class Server; // Déclaration anticipée pour éviter les problèmes d'inclusion circulaire

class Command {
public:
    virtual ~Command() {}
    virtual void execute(int clientFd, std::map<int, Client*>& clients,
                         const std::vector<std::string>& args, Server& server) = 0;
};

#endif
