// Command.hpp

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Client.hpp"
#include <vector>
#include <string>

class Command {
public:
    virtual ~Command() {}
    virtual void execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args) = 0;
};

void    sendPeriodicPong(int clientFd, std::map<int, Client*>& clients);
void    startPongService(int clientFd, std::map<int, Client*>& clients);

#endif
