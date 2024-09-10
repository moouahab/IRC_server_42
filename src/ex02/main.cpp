#include "Server.hpp"
#include <iostream>
#include <string>
#include "lib.hpp"


int main(int argc, char **argv)
{
	if (argc != 3 || !isNumber(argv[1]))
	{
		std::cerr << "Usage:./irc <port_number> <password>" << std::endl;
		return (1);
	}
	int 		port;
	std::string	password;
	if (!parseArgument(port, password, argv))
        return (-1);
	try
	{
		Server irc(password, port);
		std::cout << "le construcor est bon " << std::endl;
		irc.run();
		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
