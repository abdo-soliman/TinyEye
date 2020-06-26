#ifndef RECOGNITION_SYSTEM_H_H_
#define RECOGNITION_SYSTEM_H_H_

#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

#include "mtcnn/mtcnn.h"
#include "classifier/svm.h"
#include "mobilefacenet/mfn.h"

namespace tinyeye
{
class RecognitionSystem
{
public:
    RecognitionSystem(const RecognitionSystem&) = delete;
    static void intialize(std::string mtcnn_models_dir, std::string mfn_model_path,
                          int in_features, int out_features, std::string classifier_model_path,
                          std::string classifier_map_path, std::string camera_ip);
    static void set_frame_rate(int fr);
    static RecognitionSystem& get();
    static void camera_loop();
    static void recognition_loop();

private:
    RecognitionSystem();
    void _camera_loop();
    void _recognition_loop();

    std::unique_ptr<mtcnn::MTCNN> detector;
    std::unique_ptr<mobilefacenet::mfn> mfn_net;
    std::unique_ptr<svm> classifier;

    std::mutex img_queue_lock;
    std::mutex processed_queue_lock;

    std::queue<cv::Mat> images_buffer;
    std::queue<cv::Mat> processed_images;

    std::string ip;
    int frame_rate = 10;

    static RecognitionSystem recognition_system;
};
} // namespace tinyeye

#endif
