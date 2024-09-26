#include "CapCommand.hpp"
#include "Client.hpp"
#include <sstream>
#include <iostream>

void CapCommand::execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args, Server &server) {
    (void)server;
    
    if (args.size() < 2) {
        clients[clientFd]->messageSend("461 CAP :Not enough parameters\r\n");
        return;
    }

    std::string subCommand = args[1];

    if (subCommand == "REQ") {
        std::string capabilitiesRequested = args[2]; // Par exemple ":multi-prefix sasl"
        std::istringstream iss(capabilitiesRequested);
        std::string capability;
        std::string ackCapabilities, nakCapabilities;

        while (iss >> capability) {
            if (capability == "multi-prefix" || capability == "sasl") {
                ackCapabilities += capability + " ";
            } else {
                nakCapabilities += capability + " ";
            }
        }

        if (!ackCapabilities.empty()) {
            clients[clientFd]->messageSend("CAP * ACK :" + ackCapabilities + "\r\n");
        }
        if (!nakCapabilities.empty()) {
            clients[clientFd]->messageSend("CAP * NAK :" + nakCapabilities + "\r\n");
        }
    }
}
