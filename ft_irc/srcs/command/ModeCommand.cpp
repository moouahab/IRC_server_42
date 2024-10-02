#include "ModeCommand.hpp"
#include "OptionOperator.hpp"
#include "OptionInviteOnly.hpp"
#include "OptionKey.hpp"
#include "OptionTopicRestriction.hpp"
#include "OptionLimit.hpp"

#include "Client.hpp"
#include "Channel.hpp"
#include <iostream>
#include "Server.hpp"

ModeCommand::ModeCommand() {
    registerOption();
}

ModeCommand::~ModeCommand() {
    // Libérer les options de canal
    for (std::map<char, OptionMode *>::iterator it = _option.begin(); it != _option.end(); ++it) {
        delete it->second;
    }
}


void ModeCommand::execute(int clientFd, std::map<int, Client*>& clients,
                          const std::vector<std::string>& args, Server& server) {
    Client* client = clients[clientFd];


    if (args.size() < 2) {
        client->messageSend("461 MODE :Not enough parameters\r\n");
        return;
    }
    std::string target = args[1];
    if (server.getChannel(target)) {
        // Gestion des modes de canal
        Channel* channel = server.getChannel(target);
        if (!channel) {
            client->messageSend("403 " + target + " :No such channel\r\n");
            return;
        }

        if (args.size() == 2) {
            // Pas de modes spécifiés, renvoyer les modes actuels du canal
            client->messageSend("324 " + client->getUserName() + " " + channel->getName() + " +" + channel->getMode() + "\r\n");
            return;
        }

        std::string modes = args[2];
        size_t argIndex = 3;
        bool adding = true;

        for (size_t i = 0; i < modes.size(); ++i) {
            char modeChar = modes[i];
            if (modeChar == '+') {
                adding = true;
            } else if (modeChar == '-') {
                adding = false;
            } else {
                OptionMode* option = _option[modeChar];
                if (option) {
                    std::string modeArgument;

                    if (option->requiresArgument()) {

                        if (argIndex >= args.size() && modeChar != 'l' && modeChar != 'o') {
                            client->messageSend("461 MODE :Not enough parameters\r\n");
                            return ;
                        }
						else if (argIndex < args.size())
                        	modeArgument = args[argIndex++];
                    }

                    if (adding)
                        option->apply(channel, client, modeArgument);
                    else
                        option->remove(channel, client, modeArgument);
                } else {
                    client->messageSend("472 " + std::string(1, modeChar) + " :is unknown mode char to me\r\n");
                }

            }
        }
    }
}


void ModeCommand::registerOption() {
    _option['o'] = new OptionOperator();
    _option['i'] = new OptionInviteOnly();
    _option['k'] = new OptionKey();
    _option['t'] = new OptionTopicRestriction();
    _option['l'] = new OptionLimit();
    // Ajouter d'autres options ici
}
