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
        this->sio_client.socket(this->nsp)->on("model", std::bind(&socket::on_model, this, std::placeholders::_1));
    }

    bool socket::get_updated_model()
    {
        return updated_model;
    }

    void socket::on_model(sio::event &e)
    {
        std::cout << "downloading classifier" << std::endl;
        this->updated_model = false;
        std::string map_url = e.get_message()->get_map()["map"]->get_string();
        std::string model_url = e.get_message()->get_map()["model"]->get_string();
        utils::download(model_url, this->config["CLASSIFIER_PATH"]);
        utils::download(map_url, this->config["MAP_PATH"]);
        this->updated_model = true;
    }

    void socket::send_test()
    {
        std::string ms = "Hello world";
        std::string message = "{ \"user_id\": \"id\", \"message\": \"" + ms + "\" }";
        std::cout << message << std::endl;
        this->sio_client.socket(this->nsp)->emit("chat", message);
    }

    void socket::send_log(std::string name, std::string image, std::string type)
    {
        std::string server_log_msg = "{ \"person_name\": \"" + name + "\", ";
        server_log_msg += "\"type\": \"" + type + "\", ";
        server_log_msg += "\"image\": \"" + image + "\" }";
        this->sio_client.socket(this->nsp)->emit("log", server_log_msg);
    }

    void socket::send_get_model_signal()
    {
        this->sio_client.socket(this->nsp)->emit("get_model");
    }

} // namespace tinyeye
