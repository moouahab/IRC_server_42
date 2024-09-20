#include "CommandHandler.hpp"
#include "Command.hpp"
#include "PassCommand.hpp"
#include "PingCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"
#include "CapCommand.hpp"
#include <iostream>

CommandHandler::CommandHandler(std::map<int, Client*>& clients, const std::string& password)
    : _clients(clients), _password(password) {

    // Exemple d'enregistrement de commandes
    registerCommand("PASS", new PassCommand(password));
    registerCommand("PING", new PingCommand());
    registerCommand("NICK", new NickCommand());
    registerCommand("USER", new UserCommand());
    registerCommand("CAP", new CapCommand());
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
    if (_commands.find(name) != _commands.end()) {
        return _commands[name];
    }
    return NULL;
}

void CommandHandler::handleCommand(int clientFd, const std::vector<std::string>& args) {
    if (args.empty()) return ;

    std::string commandName = args[0];
    Command* command = createCommand(commandName);

    if (command) {
        executeCommand(command, clientFd, args);
    } else {
        _clients[clientFd]->messageSend("\033[31mUnknown command\r\n\033[0m");
    }
}

void CommandHandler::executeCommand(Command* command, int clientFd, const std::vector<std::string>& args) {
    command->execute(clientFd, _clients, args);
}
