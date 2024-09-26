#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "Command.hpp"

class ModeCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients,
                 const std::vector<std::string>& args, Server& server);
};

#endif // MODECOMMAND_HPP