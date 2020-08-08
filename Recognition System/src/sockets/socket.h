#ifndef SOCKET_H_H_
#define SOCKET_H_H_

#include <string>
#include <iostream>
#include <sio_client.h>

namespace tinyeye
{
    class socket
    {
    private:
        sio::client sio_client;
        std::map<std::string, std::string> config;
        std::string nsp;

        void OnMessage(sio::event &e);
    public:
        socket(std::map<std::string, std::string> config);
        void connect();
        void send_test();
    };

} // namespace tinyeye

#endif