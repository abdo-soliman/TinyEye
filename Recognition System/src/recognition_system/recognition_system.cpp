#include "recognition_system.h"

#include <unistd.h>
#include <iostream>

#include "utils.h"
#include "logger/logger.h"

namespace tinyeye
{
RecognitionSystem RecognitionSystem::recognition_system;

RecognitionSystem::RecognitionSystem()
{

}

void RecognitionSystem::intialize(std::string mtcnn_models_dir, std::string mfn_model_path,
                                  int in_features, int out_features, std::string classifier_model_path,
                                  std::string classifier_map_path, std::string camera_ip)
{
    try
    {
        recognition_system.detector.reset();
        recognition_system.detector = std::make_unique<mtcnn::MTCNN>(mtcnn_models_dir);
    }
    catch(const std::exception& e)
    {
        std::string error_msg = utils::replace_all(e.what(), "\n", " ");
        error_msg = utils::replace_all(e.what(), "\0", " ");
        error_msg = utils::replace_all(error_msg, "\"");
        logger::LOG_ERROR(FAILED_TO_INTIALIZE_MTCNN, error_msg);
        exit(-1);
    }

    try
    {
        recognition_system.mfn_net.reset();
        recognition_system.mfn_net = std::make_unique<mobilefacenet::mfn>(mfn_model_path);
    }
    catch(const std::exception& e)
    {
        std::string error_msg = utils::replace_all(e.what(), "\n", " ");
        error_msg = utils::replace_all(e.what(), "\0", " ");
        error_msg = utils::replace_all(error_msg, "\"");
        logger::LOG_ERROR(FAILED_TO_INTIALIZE_MOBILE_FACENET, error_msg);
        exit(-1);
    }
    
    try
    {
        recognition_system.classifier.reset();
        recognition_system.classifier = std::make_unique<svm>(in_features, out_features);
        recognition_system.classifier->construct_map(classifier_map_path);
        recognition_system.classifier->load(classifier_model_path);
    }
    catch(const std::exception& e)
    {
        std::string error_msg = utils::replace_all(e.what(), "\n", " ");
        error_msg = utils::replace_all(e.what(), "\0", " ");
        error_msg = utils::replace_all(error_msg, "\"");
        logger::LOG_ERROR(FAILED_TO_INTIALIZE_CLASSIFIER, error_msg);
        exit(-1);
    }

    recognition_system.ip = camera_ip;
}

RecognitionSystem& RecognitionSystem::get()
{
    return recognition_system;
}

void RecognitionSystem::set_frame_rate(int fr)
{
    get().frame_rate = fr;
}

void RecognitionSystem::set_max_imgs_per_temp(int max_per_temp)
{
    get().max_imgs_per_temp = max_per_temp;
}

void RecognitionSystem::set_temp_dir(std::string path)
{
    get().temp_dir = path;
}

void RecognitionSystem::recognition_loop()
{
    get()._recognition_loop();
}

void RecognitionSystem::camera_loop()
{
    get()._camera_loop();
}

void RecognitionSystem::_recognition_loop()
{
    bool empty_queue = true;
    std::string name;
    std::string command;
    std::string dir_name = "";
    std::vector<std::string> img_paths;
    std::vector<cv::Mat> imgs;
    while (true)
    {
        {
            const std::lock_guard<std::mutex> lock(processing_dirs_mutex);
            empty_queue = processing_dirs.empty();
        }
    
        if (empty_queue)
            usleep(200);
        else
        {
            {
                const std::lock_guard<std::mutex> lock(processing_dirs_mutex);
                dir_name = processing_dirs.front();
                processing_dirs.pop();
            }

            if (dir_name == "")
                continue;

            img_paths = utils::list_dir(dir_name);
            for (const auto& img_path : img_paths)
                imgs.push_back(cv::imread(img_path));

            clock_t start = clock();
            name = classifier->predict_block(mfn_net->get_embeddings(imgs));
            std::cout << "[PREDICTION] recognition_time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
            std::cout << "[PREDICTION] " << name << std::endl;

            if (name == "unknown")
                logger::LOG_INFO(DETECTED_UNKNOWN, "unknown personal detected");
            else
                logger::LOG_INFO(DETECTED_KNOWN, name + " was detected");

            imgs.clear();
            command = "rm -rf " + dir_name;
            system(command.c_str());
        }
    }
}

void RecognitionSystem::_camera_loop()
{
    // Read video
    cv::VideoCapture cap;
    std::string url = "http://" + ip + ":4747/video?x.mjpeg";

    cap.open(url);
    if (!cap.isOpened())
    {
        std::cout << "[CAMERA] can't read camera feed please make sure the ip is correct" << std::endl;
        return;
    }

    cv::Mat frame, face;
    cv::Rect2d bbox;
    cv::Rect2d* bbox_ptr;

    std::list<cv::Rect2d> correct_boxes;
    std::vector<tracker_ptr> to_delete;

    std::string dir_name;
    int count = -1;
    int x1, y1, width, height, num_files;
    while (true)
    {
        if (!cap.read(frame))
        {
            std::cout << "[CAMERA] frame droped!!!" << std::endl;
            continue;
        }

        cv::rotate(frame, frame, cv::ROTATE_90_COUNTERCLOCKWISE);

        if (++count == frame_rate)
            count = 0;

        if (count == 0)
        {
            clock_t start = clock();
            correct_boxes = detector->detect_rects(frame, 0.97);
            std::cout << "[PROCESSING] detection_time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;

            for (auto& tracker : trackers)
            {
                int overlaps = 0;
                for (auto& box : correct_boxes)
                {
                    if (overlaping(box, trackers_current_box[tracker]))
                    {
                        bbox_ptr = &box;
                        overlaps++;
                    }
                }

                if (overlaps != 1)
                {
                    tracker_done[tracker] = true;
                    to_delete.push_back(tracker);
                    std::cout << "[PROCESSING] done overlapping" << std::endl;
                }
                else
                    correct_boxes.remove(*bbox_ptr);
            }

            for (auto& box : correct_boxes)
                allocate_tracker(frame, box);
        }
        else
        {
            for (auto& tracker : trackers)
            {
                clock_t start = clock();
                if (tracker->update(frame, bbox))
                {
                    std::cout << "[PROCESSING] tracking_time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
                    trackers_current_box[tracker] = bbox;
                    if (!tracker_done[tracker])
                    {
                        dir_name = trackers_dirs_map[tracker].first;
                        num_files = trackers_dirs_map[tracker].second;

                        face = cv::Mat(frame, cv::Rect(bbox.x, bbox.y, bbox.width, bbox.height));
                        cv::imwrite(dir_name + "/" + std::to_string(num_files) + ".jpg", face);

                        trackers_dirs_map[tracker] = std::make_pair(dir_name, ++num_files);

                        if (num_files >= max_imgs_per_temp)
                        {
                            tracker_done[tracker] = true;
                            std::cout << "[PROCESSING] done max imgs count" << std::endl;
                        }
                    }
                }
                else
                {
                    tracker_done[tracker] = true;
                    to_delete.push_back(tracker);
                    std::cout << "[PROCESSING] done out of frame" << std::endl;
                }
            }
        }

        for (auto& tracker : trackers)
        {
            if (tracker_done[tracker] && !tracker_processed[tracker])
            {
                const std::lock_guard<std::mutex> lock(processing_dirs_mutex);
                processing_dirs.push(trackers_dirs_map[tracker].first);
                tracker_processed[tracker] = true;
            }
        }

        for (auto& tracker : to_delete)
        {
            trackers_current_box.erase(tracker);
            trackers_dirs_map.erase(tracker);
            tracker_done.erase(tracker);
            tracker_processed.erase(tracker);
            trackers.remove(tracker);
            std::cout << "[PROCESSING] deleting useless trackers" << std::endl;
        }
        to_delete.clear();
    }
}

bool RecognitionSystem::overlaping(const cv::Rect2d& b1, const cv::Rect2d& b2)
{
    cv::Point b1_corners[4] = {b1.tl(), b1.br()};
    b1_corners[2] = cv::Point(b1.x + b1.width, b1.y);
    b1_corners[2] = cv::Point(b1.x, b1.y + b1.height);
    
    for (int i = 0; i < 4; i++)
    {
        if ((b1_corners[i].x >= b2.x) && (b1_corners[i].x <= (b2.x + b2.width)))
        {
            if ((b1_corners[i].y >= b2.y) && (b1_corners[i].y <= (b2.y + b2.height)))
                return true;
        }
    }

    cv::Point b1_center = cv::Point(b1.x + b1.width/2, b1.y + b1.height/2);
    cv::Point b2_center = cv::Point(b2.x + b2.width/2, b2.y + b2.height/2);
    return sqrt(pow(b2_center.y - b1_center.y, 2) + pow(b2_center.x - b1_center.x, 2)) < 30;
}

void RecognitionSystem::allocate_tracker(const cv::Mat& frame, const cv::Rect2d& box)
{
    tracker_ptr tracker = cv::TrackerMedianFlow::create();
    tracker->init(frame, cv::Rect2d(box.x, box.y, box.width, box.height));
    trackers.push_back(tracker);
    trackers_current_box.insert(std::make_pair(tracker, cv::Rect2d(box.x, box.y, box.width, box.height)));
    tracker_done.insert(std::make_pair(tracker, false));
    tracker_processed.insert(std::make_pair(tracker, false));

    std::string dir_name;
    bool used = true;
    while (used)
    {
        dir_name = temp_dir + "/" + utils::random_string(dir_name_length);

        used = false;
        for (const auto& tracker_dir : trackers_dirs_map)
        {
            if (tracker_dir.second.first == dir_name)
                used = true;
        }
    }

    std::string command = "mkdir " + dir_name;
    system(command.c_str());

    auto face = cv::Mat(frame, cv::Rect(box.x, box.y, box.width, box.height));
    cv::imwrite(dir_name + "/0.jpg", face);

    trackers_dirs_map.insert(std::make_pair(tracker, std::make_pair(dir_name, 1)));
}
} // namespace tinyeye
