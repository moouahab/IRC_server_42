#ifndef CLIENT_HTTP
#define CLIENT_HTTP

#include "lib.hpp"
#include <ctime>

class Client
{
    private:
        std::string _hostName;
        std::string _userName;
        int         _sockfd;
        bool        _connected;
        bool        _isIrssi;
        std::time_t _connectTime;
        
    public:
        Client(int  sockfd);
        ~Client();

        std::string         getMessageClient();
        void                messageSend(const std::string &message);
        void               isIrssiClientConnect();


        bool        getConnect() const;
        std::string getUserName() const;
        int         getUserId() const;
        std::string getHostName() const;
        std::time_t getConnectTime() const;
        bool        isIrssi() const;
        void        setConnect(bool value);
        void        setUserName(const std::string &userName);
        void        setHostName(const std::string &hostName);

        bool        isSessionActive();
        
};

#endif