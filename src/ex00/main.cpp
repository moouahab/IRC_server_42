#include "Serveur.hpp"

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
		Serveur irc(password, port);
		irc.run();
		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
