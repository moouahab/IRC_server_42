#ifndef OPTIONTOPICRESTRICTION_HPP
#define OPTIONTOPICRESTRICTION_HPP

#include "OptionMode.hpp"

class OptionTopicRestriction : public OptionMode {
public:
    void apply(Channel* channel, Client* client, const std::string& argument);
    void remove(Channel* channel, Client* client, const std::string& argument);
    bool requiresArgument() const { return false; }
};

#endif // OPTIONTOPICRESTRICTION_HPP
