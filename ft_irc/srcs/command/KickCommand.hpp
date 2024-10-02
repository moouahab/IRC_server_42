#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "Command.hpp"

class KickCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients,
                 const std::vector<std::string>& args, Server& server);
};

#endif // KICKCOMMAND_HPP
