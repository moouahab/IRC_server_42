#ifndef OPTIONINVITEONLY_HPP
#define OPTIONINVITEONLY_HPP

#include "OptionMode.hpp"

class OptionInviteOnly : public OptionMode {
public:
    void apply(Channel* channel, Client* client, const std::string& argument);
    void remove(Channel* channel, Client* client, const std::string& argument);
    bool requiresArgument() const { return false; }
};

#endif // OPTIONINVITEONLY_HPP
