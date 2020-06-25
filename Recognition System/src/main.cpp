#include <thread>
#include "logger/logger.h"
#include "recognition_system/recognition_system.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "you must pass IP address" << std::endl;
        return -1;
    }

    tinyeye::logger::setup_server_socket("http://localhost:3000");
    tinyeye::RecognitionSystem::intialize("../../models/mtcnn", "../../models/mobilefacenet.pt", 128, 5,
                                          "../../models/svm_model.pt", "../../models/classes_map.txt", argv[1]);

    std::thread t1(&tinyeye::RecognitionSystem::camera_loop);
    std::thread t2(&tinyeye::RecognitionSystem::recognition_loop);

    t1.join();

    return 0;
}
