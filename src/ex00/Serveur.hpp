/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moouahab <mohamed.ouahab1999@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 15:44:09 by moouahab          #+#    #+#             */
/*   Updated: 2024/08/15 08:20:26 by moouahab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVEUR_HPP
#define SERVEUR_HPP


#include <iostream>
#include <string>
#include <vector>
#include "lib.hpp"



class Serveur {
	private:
		unsigned int		_port;
		std::string			_password;
		int					_listenFd;
		std::vector<pollfd> _sockFds;
		std::map<int, t_client_info> clients;

		void				accepterConnexion();
		void				gererClient(int client_fd);
		void				fermerClient(int client_fd);
	public:
		Serveur(const std::string& password, unsigned int port);

		~Serveur();
		int getListenFd() const;
		unsigned int getPort() const;
		const std::string& getPassword() const;

		void run();
};






#endif
