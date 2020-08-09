#include <thread>
#include "logger/logger.h"
#include "sockets/socket.h"
#include "argparser/argparser.h"
#include "utils.h"
#include "recognition_system/recognition_system.h"

int main(int argc, char **argv)
{
    tinyeye::ArgumentParser parser;
    parser.add_option("--core-log-path", 's', "~/tinyeye-core.log", "path for log file path");
    parser.add_option("--server-url", 's', "http://localhost:3000", "remote server url to send notifications to it");
    parser.add_option("--mtcnn-models-dir", 's', "../models/mtcnn", "MTCNN face detection framework Models dir");
    parser.add_option("--mfn-model-path", 's', "../models/mobilefacenet.pt", "MobileFacenet Model Path");
    parser.add_option("--num-classes", 'i', "5", "number of people that could be identified as known");
    parser.add_option("--classifier-model-path", 's', "../models/svm_model.pt", "path for classifier to classify mfn embeddings to their coresponding classes");
    parser.add_option("--classes-map-path", 's', "../models/classes_map.txt", "path for mapping file for classes and equivilent names");
    parser.add_option("--camera-ip", 's', "192.168.1.9", "ip for camera to capture video feed from it");
    parser.add_option("--frame-rate", 'i', "5", "video feed frame rate");
    parser.add_option("--max-imgs-per-temp", 'i', "5", "number of images stored per person for recognition");
    parser.add_option("--temp-dir", 's', "~/tinyeye_temp", "path for temp directory to store detection results for later recognition");

    parser.parse(argc, argv);
    std::map<std::string, std::string> config = utils::read_config();

    tinyeye::logger::set_logfile_path(parser.get_option<std::string>("--core-log-path"));
    std::string server_url = parser.get_option<std::string>("--server-url");
    std::string mtcnn_models_dir = parser.get_option<std::string>("--mtcnn-models-dir");
    std::string mfn_model_path = parser.get_option<std::string>("--mfn-model-path");
    int num_classes = parser.get_option<int>("--num-classes");
    std::string classifier_model_path = parser.get_option<std::string>("--classifier-model-path");
    std::string classes_map_path = parser.get_option<std::string>("--classes-map-path");
    std::string camera_ip = parser.get_option<std::string>("--camera-ip");
    int frame_rate = parser.get_option<int>("--frame-rate");
    int max_per_temp = parser.get_option<int>("--max-imgs-per-temp");
    std::string temp_dir_path = parser.get_option<std::string>("--temp-dir");

    auto *sio = new tinyeye::socket(config);

    sio->connect();

    // if (utils::exists(config["SVM_PATH"]))
    // {
    //     std::cout << "found" << std::endl;
    // }
    // else
    // {
    //     std::cout << "not found" << std::endl;
    // }

    // sleep(5);
    // sio->send_test();
    // sleep(5);

    // bool downloaded = utils::download("https://i.imgur.com/oRtvmGT.jpg", "./test.jpg");
    // if (downloaded)
    // {
    //     std::cout << "done" << std::endl;
    // }
    // else
    // {
    //     std::cout << "failed" << std::endl;
    // }

    // tinyeye::logger::setup_server_socket(server_url);
    tinyeye::RecognitionSystem::intialize(mtcnn_models_dir, mfn_model_path, 128, num_classes,
                                          classifier_model_path, classes_map_path, camera_ip, temp_dir_path, sio);
    tinyeye::RecognitionSystem::set_frame_rate(frame_rate);
    tinyeye::RecognitionSystem::set_max_imgs_per_temp(max_per_temp);

    std::thread t1(&tinyeye::RecognitionSystem::camera_loop);
    std::thread t2(&tinyeye::RecognitionSystem::detection_loop);
    std::thread t3(&tinyeye::RecognitionSystem::recognition_loop);

    t1.join();

    return 0;
}
