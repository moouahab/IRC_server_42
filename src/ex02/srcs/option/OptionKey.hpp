#ifndef OPTIONKEY_HPP
#define OPTIONKEY_HPP

#include "OptionMode.hpp"

class OptionKey : public OptionMode {
public:
    void apply(Channel* channel, Client* client, const std::string& argument);
    void remove(Channel* channel, Client* client, const std::string& argument);
    bool requiresArgument() const { return true; }
};

#endif // OPTIONKEY_HPP
