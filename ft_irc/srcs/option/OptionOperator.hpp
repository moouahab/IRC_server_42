#ifndef OPTIONOPERATOR_HPP
#define OPTIONOPERATOR_HPP

#include "OptionMode.hpp"

class OptionOperator : public OptionMode {
public:
    void apply(Channel* channel, Client* client, const std::string& argument);
    void remove(Channel* channel, Client* client, const std::string& argument);
    bool requiresArgument() const { return true; }
};

#endif // OPTIONOPERATOR_HPP
