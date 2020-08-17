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
        bool updated_model = false;

        void OnMessage(sio::event &e);
        void on_model(sio::event &e);

    public:
        socket(std::map<std::string, std::string> config);
        void connect();
        void send_get_model_signal();
        void send_test();
        void send_log(std::string name, std::string image, std::string type);
        bool get_updated_model();
    };

} // namespace tinyeye

#endif