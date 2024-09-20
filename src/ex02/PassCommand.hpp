#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "Command.hpp"
#include <string>

class PassCommand : public Command {
    public:
        PassCommand(const std::string& password) : _password(password) {}
        void execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args);

    private:
        std::string _password;
};

#endif
