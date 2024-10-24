# ft_irc

## Introduction

**ft_irc** est un projet réalisé dans le cadre du cursus de l'école 42. Il s'agit d'implémenter un serveur IRC simple en **C++98**, en suivant le protocole IRC tel que défini dans la [RFC 2812](https://datatracker.ietf.org/doc/html/rfc2812). Le projet permet à plusieurs clients de se connecter, de communiquer entre eux et de rejoindre des canaux IRC.

Le protocole IRC (Internet Relay Chat) est un protocole de communication en temps réel basé sur TCP. Les clients et le serveur échangent des messages formatés selon des règles spécifiques pour établir une connexion, s'authentifier, rejoindre des canaux, et envoyer/recevoir des messages.

## Fonctionnalités principales

- Serveur IRC multi-clients
- Support de plusieurs canaux IRC
- Gestion de commandes IRC standard : `JOIN`, `PART`, `PRIVMSG`, `NICK`, `QUIT`
- Gestion de la communication réseau via sockets en **C++98** (utilisation de `select()` pour la gestion des événements I/O)
- Authentification basique par mot de passe

## Prérequis

- **C++98**
- Système Unix-based (Linux, MacOS)
- Utilisation de bibliothèques système pour la gestion des sockets

## Compilation et exécution

Pour compiler le projet :

```bash
make
```
Pour lancer le serveur :

```bash
./ircserv <port> <password>
```
    port : le port sur lequel le serveur va écouter les connexions (par exemple, 6667 pour IRC).
    password : mot de passe que les clients doivent fournir pour se connecter au serveur.

Protocole IRC - Détails techniques
### 1. Architecture du serveur IRC

Le serveur IRC est responsable de la gestion des connexions clients, du traitement des commandes IRC, et de la transmission des messages entre les utilisateurs. Voici un schéma simple du flux de communication :

    Connexion TCP : Le client ouvre une connexion avec le serveur sur un port spécifique (souvent 6667 pour IRC).
    Handshake : Le client s'authentifie auprès du serveur en envoyant les commandes NICK et USER.
    Communication en temps réel : Les messages sont envoyés entre les clients via le serveur.
    Déconnexion : Le client envoie une commande QUIT pour se déconnecter proprement.

### 2. Format des messages IRC

Tous les messages échangés entre le client et le serveur sont des lignes de texte terminées par \r\n. Chaque message suit un format général, qui peut être décrit comme suit :

php

<préfixe> <commande> <paramètres> :<trailing>

    Préfixe (optionnel) : Identifie l'origine du message (serveur ou utilisateur).
    Commande : Une des nombreuses commandes IRC (ex. JOIN, PRIVMSG, NICK).
    Paramètres : Paramètres associés à la commande (ex. nom de canal, message).
    Trailing (facultatif) : Texte libre, souvent utilisé pour le contenu des messages.

Exemple de message d'un client à un canal IRC :

arduino

PRIVMSG #channel :Hello, everyone!

### 3. Commandes IRC prises en charge

Voici les principales commandes que votre serveur IRC doit implémenter :

    NICK <nickname> : Définit ou change le pseudonyme d'un utilisateur.
        Ex : NICK JohnDoe
    USER <username> <hostname> <servername> <realname> : Enregistre un nouveau client sur le serveur.
        Ex : USER john localhost irc.example.com :John Doe
    JOIN <channel> : Rejoint un canal spécifique, ou en crée un s'il n'existe pas.
        Ex : JOIN #42network
    PART <channel> : Quitte un canal.
        Ex : PART #42network
    PRIVMSG <target> :<message> : Envoie un message privé à un utilisateur ou à un canal.
        Ex : PRIVMSG JohnDoe :Hey, how's it going?
    NOTICE <target> :<message> : Envoie un message sans attendre de réponse.
    QUIT <message> : Déconnecte le client du serveur, avec un message facultatif.
        Ex : QUIT :Goodbye!

### 4. Gestion des connexions clients

Le serveur utilise les sockets TCP pour établir une connexion avec chaque client. Voici un aperçu de la gestion des connexions dans le projet ft_irc :

    Création de la socket du serveur : Le serveur crée une socket et la lie à un port spécifique en écoutant les connexions entrantes.

    Attente de connexions avec select() : Le serveur utilise la fonction select() pour surveiller plusieurs sockets simultanément (chaque client étant lié à une socket unique). Cela permet de gérer efficacement plusieurs connexions sans bloquer.

    Acceptation des nouvelles connexions : Lorsqu'un nouveau client tente de se connecter, le serveur accepte la connexion avec accept(). Une nouvelle socket est alors créée pour ce client.

    Lecture/écriture des messages : Les messages entrants des clients sont lus à travers les sockets avec recv() et traités en fonction de la commande reçue (par exemple, JOIN, PRIVMSG).

### 5. Transmission des messages

Le serveur reçoit des messages des clients via des sockets et les redirige vers les destinataires appropriés (autres clients ou canaux). Par exemple :

    Si un client envoie un message PRIVMSG à un canal, le serveur renvoie ce message à tous les autres clients connectés à ce canal.

Exemple de transmission de message :

bash

Client 1: PRIVMSG #42network :Hello, world!
Serveur -> Envoi du message à tous les clients dans le canal #42network

### 6. Authentification et gestion des erreurs

Lors de la connexion, le client doit envoyer une combinaison valide de NICK et USER pour être authentifié sur le serveur. Si l'utilisateur ne respecte pas ce protocole, le serveur répond avec un code d'erreur.

Quelques erreurs courantes :

    ERR_NICKNAMEINUSE (433) : Le pseudonyme choisi est déjà utilisé.
    ERR_NEEDMOREPARAMS (461) : Paramètres manquants dans une commande.
    ERR_NOTREGISTERED (451) : Tentative d'exécution d'une commande sans s'être correctement authentifié.

Le serveur doit renvoyer les bons codes d'erreur selon le contexte, pour que les clients sachent comment corriger leurs actions.
Conclusion

Le projet ft_irc implémente les bases d'un serveur IRC en suivant les standards définis dans la RFC 2812. La communication se fait via des sockets TCP, avec une gestion efficace des connexions multiples grâce à select(). Les commandes IRC essentielles sont supportées, permettant ainsi aux clients de rejoindre des canaux et d'échanger des messages en temps réel.

Ce serveur peut être étendu avec des fonctionnalités supplémentaires pour implémenter un serveur IRC complet et conforme aux standards, telles que la gestion des modes de canal, les opérateurs, et d'autres fonctionnalités avancées.

---

Cette version tient bien compte de l'utilisation de **C++98**. Le projet fonctionne via les sockets TCP et utilise la fonction `select()` pour la gestion des événements d'I/O, une technique adaptée à la norme C++98. Si tu as besoin d'ajouter ou modifier des détails, n'hésite pas à me le dire !
