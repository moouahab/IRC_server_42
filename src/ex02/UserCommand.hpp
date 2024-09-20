#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "Command.hpp"
#include <string>

class UserCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args);
};

#endif