#ifndef WHOISCOMMAND_HPP
#define WHOISCOMMAND_HPP

#include "Command.hpp"
#include <string>

class WhoisCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args);
};

#endif // WHOISCOMMAND_HPP
