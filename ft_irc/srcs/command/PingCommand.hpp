#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "Command.hpp"

class PingCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients,
                 const std::vector<std::string>& args, Server& server);
};

#endif
