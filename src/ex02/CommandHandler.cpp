#include "CommandHandler.hpp"
#include "PassCommand.hpp"
#include "PingCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"
#include "CapCommand.hpp"
#include "PrivmsgCommand.hpp"
#include "WhoisCommand.hpp"
#include "JoinCommand.hpp"
#include <iostream>

CommandHandler::CommandHandler(Server& server, std::map<int, Client*>& clients, const std::string& password)
    : _server(server), _clients(clients), _password(password) {

    // Enregistrement des commandes
    registerCommand("PASS", new PassCommand(password));
    registerCommand("PING", new PingCommand());
    registerCommand("NICK", new NickCommand());
    registerCommand("USER", new UserCommand());
    registerCommand("WHOIS", new WhoisCommand());
    registerCommand("CAP", new CapCommand());
    registerCommand("PRIVMSG", new PrivmsgCommand());
    registerCommand("JOIN", new JoinCommand());
}

CommandHandler::~CommandHandler() {
    // Supprimer toutes les commandes enregistrées
    for (std::map<std::string, Command*>::iterator it = _commands.begin(); it != _commands.end(); ++it) {
        delete it->second;
    }
}

void CommandHandler::registerCommand(const std::string& name, Command* command) {
    _commands[name] = command;
}

Command* CommandHandler::createCommand(const std::string& name) {
    // Vérifier si la commande existe dans _commands
    if (_commands.find(name) != _commands.end())
        return _commands[name];
    return NULL;
}

void CommandHandler::handleCommand(int clientFd, const std::vector<std::string>& args) {
    // Vérifier si la commande est présente dans les arguments
    if (args.empty()) return ;
    std::cout << "[DEBUG] ";
    for (size_t i = 0; i < args.size(); i++)
    {
        std::cout << args[i] << " ";
    }
    std::cout << std::endl;

    std::string     commandName = args[0];
    Command*        command = createCommand(commandName);

    if (command)
        executeCommand(command, clientFd, args);
    else
         _clients[clientFd]->messageSend("421 " + args[0] + " :Unknown command\r\n");
}

void CommandHandler::executeCommand(Command* command, int clientFd, const std::vector<std::string>& args) {
    command->execute(clientFd, _clients, args, _server);
}
