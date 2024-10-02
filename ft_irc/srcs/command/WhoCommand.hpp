#ifndef WHOCOMMAND_HPP
#define WHOCOMMAND_HPP

#include "Command.hpp"

class WhoCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients,
                 const std::vector<std::string>& args, Server& server);
};

#endif // WHOCOMMAND_HPP
