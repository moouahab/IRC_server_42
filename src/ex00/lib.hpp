#ifndef LIB_HPP
# define LIB_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <stdexcept>
# include <cstring>
# include <poll.h>

bool    isNumber(const std::string &str);
bool    isUnderscoreOrHyphen(char c);

bool    passwordValid(const std::string& str);
bool    parseArgument(int &port, std::string &password, char *av[]);

#endif