#ifndef CLIENT_HTTP
#define CLIENT_HTTP

#include "lib.hpp"
#include <ctime>
#include <set>

class Channel; // Déclaration anticipée

class Client
{
    private:
        int                 _sockfd;
        std::string         _hostName;
        std::string         _userName;
        bool                _connected;
        bool                _isIrssi;
        std::string         _realName;
        std::time_t         _connectTime;
        std::set<Channel*>  _channels;

    public:
        Client(int  sockfd);
        ~Client();

        std::string         getMessageClient();
        void                messageSend(const std::string &message);
        bool                isIrssiClientConnect();

        std::string getUserName() const;
        std::string getHostName() const;
        std::time_t getConnectTime() const;
        std::string getRealName() const { return _realName; };
        bool        isIrssi() const;
        bool        getConnect() const;
        int         getUserId() const;

        void        setConnect(bool value);
        void        setUserName(const std::string &userName);
        void        setHostName(const std::string &hostName);
        void        setUserId(int value) { _sockfd = value;};
        void        setRealName(const std::string& realName) { _realName = realName; };

        bool        isSessionActive();
        std::string getPrefix() const;
        bool        isRegistered() const { return !_userName.empty() && !_hostName.empty(); };


        // Gestion des canaux
        void joinChannel(Channel* channel);
        void leaveChannel(Channel* channel);
        bool isInChannel(const std::string& channelName) const;
        std::set<Channel*> getChannels() const;
};

#endif
