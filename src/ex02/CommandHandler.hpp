#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#include <vector>
#include <map>
#include "Client.hpp"

class CommandHandler {
public:
    CommandHandler(std::map<int, Client*>& clients, const std::string& password);
    ~CommandHandler();

    void handleCommand(int clientFd, const std::vector<std::string>& args);

private:
    std::map<int, Client*>& _clients;
    std::string _password;

    void handlePass(int clientFd, const std::vector<std::string>& args);
    void handlePing(int clientFd, const std::vector<std::string>& args);
    void handleNick(int clientFd, const std::vector<std::string>& args);
    void handleUser(int clientFd, const std::vector<std::string>& args);
};

#endif