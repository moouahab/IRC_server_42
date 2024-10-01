#include "CommandHandler.hpp"
#include "PassCommand.hpp"
#include "PingCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"
#include "CapCommand.hpp"
#include "PrivmsgCommand.hpp"
#include "WhoisCommand.hpp"
#include "JoinCommand.hpp"
#include "ModeCommand.hpp"
#include "WhoCommand.hpp"
#include "InviteCommand.hpp"
#include "KickCommand.hpp"
#include "TopicCommand.hpp"
#include "QuitCommand.hpp"
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
    registerCommand("PART", new PartCommand());
    registerCommand("MODE", new ModeCommand());
    registerCommand("WHO", new WhoCommand());

    // Enregistrement des nouvelles commandes
    registerCommand("INVITE", new InviteCommand());
    registerCommand("KICK", new KickCommand());
    registerCommand("TOPIC", new TopicCommand());
    registerCommand("QUIT", new QuitCommand());
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
    
    if (commandName == "PASS" || commandName == "NICK" || commandName == "USER" || commandName == "CAP") {
        
        if (commandName == "NICK" && _clients[clientFd]->getConnect())
            executeCommand(command, clientFd, args);
        else if (commandName == "NICK" && !_clients[clientFd]->getConnect())
            _clients[clientFd]->messageSend("433 " + args[1] + " :Nickname is already in use\r\n");
        else if (commandName == "USER" && !_clients[clientFd]->getUserName().empty())
            executeCommand(command, clientFd, args);
        else if (commandName == "USER" && _clients[clientFd]->getUserName().empty())
            _clients[clientFd]->messageSend("433 " + args[0] + " :Nickname is already in use\r\n");
        else
            executeCommand(command, clientFd, args);
    }
    else if (!_clients[clientFd]->getHostName().empty() && !_clients[clientFd]->getUserName().empty()) {
	    if (command)
		    executeCommand(command, clientFd, args);
	    else
		    _clients[clientFd]->messageSend("421 " + args[0] + " :Unknown command\r\n");
    }
    else {
        _clients[clientFd]->messageSend("451 " + args[0] + " :Not yet registered\r\n");
    }
}

void CommandHandler::executeCommand(Command* command, int clientFd, const std::vector<std::string>& args) {
    command->execute(clientFd, _clients, args, _server);
}
