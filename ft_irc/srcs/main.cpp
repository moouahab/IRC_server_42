#include "Server.hpp"
#include <iostream>
#include <string>
#include <csignal>
#include "lib.hpp"

// Déclaration d'un pointeur global vers l'instance de serveur pour le gestionnaire de signaux
Server* globalServer = NULL;

// Gestionnaire de signaux pour fermer le serveur proprement
extern "C" void signalHandler(int signum) {
    std::cout << "\nSignal (" << signum << ") reçu. Fermeture du serveur..." << std::endl;

    if (globalServer != NULL) {
        globalServer->endServer(); // Méthode de fermeture propre que tu dois ajouter dans `Server`
    }
}

int main(int argc, char **argv)
{
    if (argc != 3 || !isNumber(argv[1]))
    {
        std::cerr << "Usage: ./irc <port_number> <password>" << std::endl;
        return 1;
    }

    int         port;
    std::string password;
    if (!parseArgument(port, password, argv))
        return -1;

    try
    {
        // Création du serveur IRC
        Server irc(password, port, "localhost");
        globalServer = &irc; // Initialiser le pointeur global pour le gestionnaire de signaux

        // Associer la fonction signalHandler aux signaux
        std::signal(SIGINT, signalHandler);  // Ctrl+C
        std::signal(SIGTERM, signalHandler); // Terminaison propre
        std::signal(SIGQUIT, signalHandler); // Ctrl+

        std::cout << "Serveur IRC en cours d'exécution. Appuyez sur Ctrl+C pour le fermer." << std::endl;

        irc.run();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
