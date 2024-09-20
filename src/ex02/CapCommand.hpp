#ifndef CAPCOMMAND_HPP
#define CAPCOMMAND_HPP

#include "Command.hpp"
#include <string>
#include <vector>
#include <map>

class CapCommand : public Command {
public:
    // Constructeur et destructeur
    ~CapCommand() {}
    // Méthode pour exécuter la commande CAP
    void execute(int clientFd, std::map<int, Client*>& clients, const std::vector<std::string>& args);
};

#endif // CAPCOMMAND_HPP
