#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Client.hpp"
#include "Command.hpp"
#include <map>
#include <string>
#include <vector>

class CommandHandler {
public:
    CommandHandler(std::map<int, Client*>& clients, const std::string& password);
    ~CommandHandler();

    void handleCommand(int clientFd, const std::vector<std::string>& args);

private:
    std::map<int, Client*>& _clients;
    std::string _password;

    std::map<std::string, Command*> _commands;

    void registerCommand(const std::string& name, Command* command);
    Command* createCommand(const std::string& name);

    void executeCommand(Command* command, int clientFd, const std::vector<std::string>& args);
};

#endif
