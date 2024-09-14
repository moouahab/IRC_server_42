#ifndef CLIENT_HTTP
#define CLIENT_HTTP

#include "lib.hpp"
#include <ctime>

class Client
{
    private:
        std::string _hostName;
        std::string _userName;
        int const   _sockfd;
        bool        _connected;
        std::time_t _connectTime;
        
    public:
        Client(int  sockfd);
        ~Client();

        std::string        getMessageClient();
        void               messageSend(const std::string &message);


        bool        getConnect() const;
        std::string getUserName() const;
        std::string getHostName() const;
        std::time_t getConnectTime() const;

        void        setConnect(bool value);
        void        setUserName(const std::string &userName);
        void        setHostName(const std::string &hostName);
};

#endif