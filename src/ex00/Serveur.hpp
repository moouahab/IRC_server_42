/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Serveur.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moouahab <moouahab@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 15:44:09 by moouahab          #+#    #+#             */
/*   Updated: 2024/08/13 19:59:56 by moouahab         ###   ########.fr       */
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
		unsigned int	_port;
		std::string		_password;
		int				_listenFd;
		void 			accepterConnexion(std::vector<pollfd>  &fds);
		void gérerClient(std::vector<pollfd>& fds, size_t index);
		
	public:
		Serveur(const std::string& password, unsigned int port);

		~Serveur();
		int getListenFd() const;
		unsigned int getPort() const;
		const std::string& getPassword() const;

		void run();
};






#endif
