#ifndef LIB_HPP
#define LIB_HPP

# include <netinet/in.h>
# include <poll.h>
# include <stdexcept>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>

# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <cstring>

# include <string>
# include <iostream>
# include <vector>
# include <poll.h>
# include <map>
# include "Logger.hpp"

bool                     isNumber(const std::string &str);
std::vector<std::string> splitString(const std::string& str, char delimiter);
bool                     parseArgument(int &port, std::string &password, char *av[]);
std::string              trim(const std::string& str);
bool matchWildcard(const std::string& pattern, const std::string& str);

#endif 