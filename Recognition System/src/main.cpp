#include <thread>
#include "logger/logger.h"
#include "sockets/socket.h"
#include "argparser/argparser.h"
#include "utils.h"
#include "recognition_system/recognition_system.h"

int main(int argc, char **argv)
{
    tinyeye::ArgumentParser parser;
    parser.add_option("--num-classes", 'i', "5", "number of people that could be identified as known");
    parser.add_option("--frame-rate", 'i', "5", "video feed frame rate");
    parser.add_option("--max-imgs-per-temp", 'i', "5", "number of images stored per person for recognition");

    parser.parse(argc, argv);
    std::map<std::string, std::string> config = utils::read_config();

    tinyeye::logger::set_logfile_path(config["CORE_LOG_PATH"]);
    std::string server_url = config["SERVER_URL"];
    std::string mtcnn_models_dir = config["MTCNN_MODELS_DIR"];
    std::string mfn_model_path = config["MFN_MODEL_DIR"];
    int num_classes = parser.get_option<int>("--num-classes");
    std::string classifier_model_path = config["CLASSIFIER_MODEL_PATH"];
    std::string classes_map_path = config["CLASSIFIER_MAP_PATH"];
    std::string camera_ip = config["CAMERA_IP"];
    int frame_rate = parser.get_option<int>("--frame-rate");
    int max_per_temp = parser.get_option<int>("--max-imgs-per-temp");
    std::string temp_dir_path = config["TEMP_DIR_PATH"];

    auto *sio = new tinyeye::socket(config);

    sio->connect();
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
