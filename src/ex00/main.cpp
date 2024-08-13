#include "Serveur.hpp"

int main()
{
	try
	{
		Serveur irc("amin\n", 5500);
		irc.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	

}