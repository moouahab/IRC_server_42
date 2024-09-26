#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include "Client.hpp"

class Channel {
    public:
        Channel(const std::string& name, Client* creator, const std::string& password = "");
        ~Channel();

        // Méthodes de gestion des clients
        void addClient(Client* client);
        void removeClient(Client* client);
        bool isClientInChannel(Client* client) const;
        std::set<Client*> getClients() const;

        // Gestion des opérateurs
        void addOperator(Client* client);
        void removeOperator(Client* client);
        bool isOperator(Client* client) const;

        // Gestion du sujet (topic)
        void setTopic(const std::string& topic);
        std::string getTopic() const;

        // Gestion des modes et mot de passe
        void setMode(const std::string& mode);
        std::string getMode() const;

        // Gestion du mot de passe
        bool isPasswordProtected() const;
        bool checkPassword(const std::string& password) const;

        // Informations sur le canal
        std::string getName() const;
        Client* getCreator() const;

        // Diffusion de messages
        void broadcast(const std::string& message, Client* sender = NULL);

    private:
        std::string       _name;
        std::string       _topic;
        std::string       _mode;
        Client*           _creator;
        
        std::string       _password;

        std::set<Client*> _clients;
        std::set<Client*> _operators;
};

#endif
