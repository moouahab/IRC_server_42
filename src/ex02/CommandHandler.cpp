#include "CommandHandler.hpp"
#include "Command.hpp"
#include "PassCommand.hpp"
#include <ctime>
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
    if (_commands.find(name) != _commands.end())
        return _commands[name];
    return NULL;
}

void CommandHandler::handleCommand(int clientFd, const std::vector<std::string>& args) {
    // Vérifier si la commande est présente dans les arguments
    if (args.empty()) return ;
    std::string commandName = args[0];
    std::time_t end = std::time(NULL);
    Command     *command = createCommand(commandName);

    std::cout << "[DEBUG] what time is it : " << std::difftime(end, _clients[clientFd]->getConnectTime()) << std::endl;
    if (command)
        executeCommand(command, clientFd, args);
    // Vérifier si 200 secondes se sont écoulées depuis la connexion du client
    // else if (std::difftime(end, _clients[clientFd]->getConnectTime()) >= 20) {
    //     // Envoyer un PING pour vérifier la connexion
    //     std::cout << "Envoyer un PING pour vérifier la connexion " << _clients[clientFd]->getConnectTime() << std::endl;
    //     Command* ping = createCommand("PING");
    //     if (ping) {
    //         // Création du vecteur d'arguments avec la syntaxe C++98
    //         std::vector<std::string> pingArgs;
    //         pingArgs.push_back("PING");
    //         pingArgs.push_back("localhost");

    //         executeCommand(ping, clientFd, pingArgs);
    //     }
    // }
    // Si aucune commande valide, envoyer un message d'erreur
    else
        _clients[clientFd]->messageSend("\033[31mUnknown command\r\n\033[0m");
}



void CommandHandler::executeCommand(Command* command, int clientFd, const std::vector<std::string>& args) {
    command->execute(clientFd, _clients, args);
}
