#include "socket.h"
#include "utils.h"

namespace tinyeye
{
    socket::socket(std::map<std::string, std::string> config)
    {
        this->config = config;
        this->nsp = "/board-" + config["SERIAL"];
    }
    void socket::connect()
    {
        std::cout << this->nsp << std::endl;
        std::map<std::string, std::string> query = {{"serial", this->config["SERIAL"]}};
        this->sio_client.connect(this->config["IP"], query);
        // this->sio_client.socket(this->nsp)->on("connection", [&](sio::event &e) {
        //     std::cout << "server: " << e.get_message()->get_string() << std::endl;
        // });
        // this->nsp = "board-" + this->config["SERIAL"];
    }

    void socket::send_test()
    {
        std::string ms = "Hello world";
        std::string message = "{ \"user_id\": \"id\", \"message\": \"" + ms + "\" }";
        std::cout << message << std::endl;
        this->sio_client.socket(this->nsp)->emit("chat", message);
    }

    // void socket::OnMessage(sio::event &e)
    // {
    // }

} // namespace tinyeye

// #include <string>
// #include <iostream>
// #include <sio_client.h>

// void OnMessage(sio::event &e)
// {
//     std::cout << "server: " << e.get_message()->get_string() << std::endl;
// }

// void OnConnect(sio::event &e)
// {
//     std::cout << "connected\n";
// }

// void OnDisconnect(sio::client::close_reason const& reason)
// {
//     std::cout << "disconnected\n";
// }

// int main(int argc, char **argv)
// {
//     if (argc < 2)
//     {
//         std::cout << "server url must be passed" << std::endl;
//         return -1;
//     }

//     sio::client h;

//     h.set_open_listener([&]() {
//         std::cout << "connected\n";
//         std::cout << h.opened() << std::endl;
//     });

//     h.set_close_listener(&OnDisconnect);
//     std::map<std::string,std::string> query = {{ "token", "123456789" }};
//     h.connect(argv[1], query);
//     // h.socket("/dynamic-11100000000000000")->on("connection", &OnConnect);
//     h.socket("/dynamic-11100000000000000")->on("chat", &OnMessage);
//     std::string id = "user1";
//     std::string message;
//     while (true)
//     {
//         std::cout << "me: ";
//         std::cin >> message;

//         message = "{ \"user_id\": \"" + id + "\", \"message\": \"" + message + "\" }";
//         h.socket("/dynamic-11100000000000000")->emit("chat", message);
//     }

//     return 0;
// }
