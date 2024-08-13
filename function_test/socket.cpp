/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moouahab <moouahab@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 13:42:30 by moouahab          #+#    #+#             */
/*   Updated: 2024/08/13 13:59:28 by moouahab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h> // Définit les types de données utilisés dans les sockets.
#include <sys/socket.h> // Contient les définitions pour les fonctions de sockets.
#include <netinet/in.h> // Définit les structures pour les adresses Internet (IPv4).
#include <arpa/inet.h> // Fournit des fonctions pour manipuler les adresses IP.
#include <unistd.h> // Contient les définitions pour des fonctions comme close().

int main()
{
	/**
	 * Création d'un socket.
	 * AF_INET spécifie que le domaine de communication est IPv4.
	 * SOCK_STREAM indique que c'est un socket pour une communication en flux (TCP).
	 * 0 signifie que le protocole par défaut est utilisé pour le type de socket donné (TCP pour SOCK_STREAM).
	 * La fonction `socket()` renvoie un descripteur de fichier (socketfd) qui représente le socket.
	 */
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (socketfd  < 0)
	{
		std::cout << "Socket non valide !!" << std::endl;
		return -1;
	}

	/**
	 * Configuration de l'adresse du serveur.
	 * `sin_family` est défini à AF_INET pour indiquer l'utilisation du protocole IPv4.
	 * `sin_addr.s_addr` est initialisé à INADDR_ANY pour accepter les connexions sur toutes les interfaces réseau disponibles.
	 * `sin_port` définit le port sur lequel le serveur écoutera les connexions entrantes, ici converti en format réseau avec `htons()`.
	 */
	struct sockaddr_in serv_adrr;
	serv_adrr.sin_family = AF_INET;
	serv_adrr.sin_addr.s_addr = INADDR_ANY;
	serv_adrr.sin_port = htons(80); // Le port est fixé à 80 (port HTTP standard).

	/**
	 * Liaison du socket à l'adresse spécifiée.
	 * La fonction `bind()` associe le socket à l'adresse et au port spécifiés dans `serv_adrr`.
	 * Si `bind()` échoue, une erreur est affichée, le socket est fermé, et le programme retourne -1.
	 */
	if (bind(socketfd, (struct sockaddr *)&serv_adrr, sizeof(serv_adrr)) < 0)
	{
		std::cout << "Erreur, la connexion n'a pas pu se faire !!" << std::endl;
		close(socketfd);
		return -1;
	}
	else
		std::cout << "La connexion est faite !!" << std::endl;

	return 0;
}

