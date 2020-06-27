#ifndef RECOGNITION_SYSTEM_H_H_
#define RECOGNITION_SYSTEM_H_H_

#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

#include "mtcnn/mtcnn.h"
#include "classifier/svm.h"
#include "mobilefacenet/mfn.h"

typedef cv::Ptr<cv::Tracker> tracker_ptr;
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
    static void set_temp_dir(std::string path);
    static RecognitionSystem& get();
    static void camera_loop();
    static void recognition_loop();

private:
    RecognitionSystem();
    void _camera_loop();
    void _recognition_loop();
    bool overlaping(const cv::Rect2d& b1, const cv::Rect2d& b2);
    void allocate_tracker(const cv::Mat& frame, const cv::Rect2d& box);

    std::unique_ptr<mtcnn::MTCNN> detector;
    std::unique_ptr<mobilefacenet::mfn> mfn_net;
    std::unique_ptr<svm> classifier;

    std::mutex processing_dirs_mutex;

    std::list<tracker_ptr> trackers;
    std::map<tracker_ptr, cv::Rect2d> trackers_current_box;
    std::map<tracker_ptr, std::pair<std::string, int>> trackers_dirs_map;
    std::map<tracker_ptr, bool> tracker_done;
    std::map<tracker_ptr, bool> tracker_processed;

    std::queue<std::string> processing_dirs;

    std::string ip;
    std::string temp_dir = "./tinyeye_temp";
    int frame_rate = 10;
    int dir_name_length = 16;
    int max_imgs_per_temp = 10;

    static RecognitionSystem recognition_system;
};
} // namespace tinyeye

#endif
