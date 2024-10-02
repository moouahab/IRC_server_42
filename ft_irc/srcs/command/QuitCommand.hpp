#ifndef QUITCOMMAND_HPP
#define QUITCOMMAND_HPP

#include "Command.hpp"

class QuitCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients,
                 const std::vector<std::string>& args, Server& server);
};

#endif // QUITCOMMAND_HPP
