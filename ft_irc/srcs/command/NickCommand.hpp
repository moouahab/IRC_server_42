// NickCommand.hpp

#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include "Command.hpp"
#include <string>

class NickCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server& server);
};

#endif
