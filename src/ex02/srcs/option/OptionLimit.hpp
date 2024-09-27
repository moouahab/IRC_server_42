#ifndef OPTIONLIMIT_HPP
#define OPTIONLIMIT_HPP

#include "OptionMode.hpp"

class OptionLimit : public OptionMode {
public:
    void apply(Channel* channel, Client* client, const std::string& argument);
    void remove(Channel* channel, Client* client, const std::string& argument);
    bool requiresArgument() const { return true; }
};

#endif // OPTIONLIMIT_HPP
