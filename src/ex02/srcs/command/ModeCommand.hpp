#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "Command.hpp"
#include <map>
#include <string>
#include "OptionMode.hpp"
// #include "OptionUserMode.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

// class OptionMode; // Déclaration anticipée
// class Client; // Déclaration anticipée
// class Server; // Déclaration anticipée

class ModeCommand : public Command {
public:
    ModeCommand();
    ~ModeCommand();
    void execute(int clientFd, std::map<int, Client*>& clients,
                 const std::vector<std::string>& args, Server& server);
private:
    std::map<char, OptionMode *> _option;
    void registerOption();
};


#endif // MODECOMMAND_HPP