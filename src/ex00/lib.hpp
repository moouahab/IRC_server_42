#ifndef LIB_HPP
# define LIB_HPP

# include <arpa/inet.h>
# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <fcntl.h>
# include <map>
# include <netinet/in.h>
# include <poll.h>
# include <stdexcept>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>

typedef enum    e_etat_client
{
	EN_ATTENTE_DE_MOT_DE_PASSE,
	AUTHENTIFIÉ,
	FERMÉ
}t_etat_client;

typedef struct			s_client_info
{
	int fd; // descripteur de fichier du client
	e_etat_client	etat;
	int tentatives;     // nombre de tentatives de mot de passe
	std::string buffer; // tampon pour les données reçues
    std::string ip;
}t_client_info;

bool			isNumber(const std::string &str);
bool			isUnderscoreOrHyphen(char c);

bool			passwordValid(const std::string &str);
bool			parseArgument(int &port, std::string &password, char *av[]);

#endif