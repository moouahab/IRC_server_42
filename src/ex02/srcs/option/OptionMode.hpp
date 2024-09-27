#ifndef OPTIONMODE_HPP
#define OPTIONMODE_HPP

#include <string>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

class OptionMode {
public:
    virtual ~OptionMode() {}

    // Méthode virtuelle pure pour appliquer le mode
    virtual void apply(Channel* channel, Client* client, const std::string& argument) = 0;

    // Méthode virtuelle pure pour retirer le mode
    virtual void remove(Channel* channel, Client* client, const std::string& argument) = 0;

    // Indique si l'option nécessite un argument
    virtual bool requiresArgument() const = 0;
};

#endif // OPTIONMODE_HPP
