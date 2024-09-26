#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "Command.hpp"

class PrivmsgCommand : public Command {
public:
    void execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args,Server &server);
};

#endif
