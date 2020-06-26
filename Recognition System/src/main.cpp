#include <thread>
#include "logger/logger.h"
#include "argparser/argparser.h"
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

    parser.parse(argc, argv);

    tinyeye::logger::set_logfile_path(parser.get_option<std::string>("--core-log-path"));
    std::string server_url = parser.get_option<std::string>("--server-url");
    std::string mtcnn_models_dir = parser.get_option<std::string>("--mtcnn-models-dir");
    std::string mfn_model_path = parser.get_option<std::string>("--mfn-model-path");
    int num_classes = parser.get_option<int>("--num-classes");
    std::string classifier_model_path = parser.get_option<std::string>("--classifier-model-path");
    std::string classes_map_path = parser.get_option<std::string>("--classes-map-path");
    std::string camera_ip = parser.get_option<std::string>("--camera-ip");

    tinyeye::logger::setup_server_socket(server_url);
    tinyeye::RecognitionSystem::intialize(mtcnn_models_dir, mfn_model_path, 128, num_classes,
                                          classifier_model_path, classes_map_path, camera_ip);

    std::thread t1(&tinyeye::RecognitionSystem::camera_loop);
    std::thread t2(&tinyeye::RecognitionSystem::recognition_loop);

    t1.join();

    return 0;
}
