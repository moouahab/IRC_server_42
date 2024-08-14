#include <iostream>
#include <cctype>
#include <string>
#include <cstring>
#include <algorithm>

bool isNumber(const std::string &str)
{
    for (size_t i = 0; i < str.size(); i++)
        if (!std::isdigit(str[i]))
            return (false);
    return (true);
}


bool isUnderscoreOrHyphen(char c)
{
    return c == '_' || c == '-';
}

bool passwordValid(const std::string& str)
{
    if(str.size() < 8)
        return false;
    if(std::count_if(str.begin(), str.end(), ::isupper) < 1)
        return false;
    if(std::count_if(str.begin(), str.end(), ::islower) < 1)
        return false;
    if(std::count_if(str.begin(), str.end(), ::isdigit) < 1)
        return false;
    if(std::count_if(str.begin(), str.end(), isUnderscoreOrHyphen) < 1)
        return false;
    return true;
}

bool    parseArgument(int &port, std::string &password, char *av[])
{
   port = std::atol(av[1]);
	if (port > 65535)
	{
		std::cerr << "Invalid port number. It should be between 1 and 65535." << std::endl;
		return (false);
	}
	password = av[2];
    if (password.find("\n") != std::string::npos)
    {
        std::cerr << "Password should not contain newline character." << std::endl;
        password.erase(password.find("\n"));
    }
	if (!passwordValid(password))
	{
		std::cerr << "Password is not valid." << std::endl;
		return (false);
	}
    return (true);
}
