#include "recognition_system.h"

#include <map>
#include <unistd.h>
#include <iostream>
#include <opencv2/tracking.hpp>

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
        recognition_system.mfn.reset();
        recognition_system.mfn = std::make_unique<mobilefacenet::MobileFacenet>(mfn_model_path);
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
    cv::Rect2d bbox;
    cv::Mat frame;

    std::vector<mtcnn::Bbox> boxes;
    std::vector<cv::Ptr<cv::Tracker>> trackers;
    std::vector<cv::Mat> processing_buffer;
    std::map<int, std::vector<cv::Mat>> faces_buffer;

    std::string name;
    int x1, y1, width, height, count;
    while (true)
    {
        trackers.clear();
        faces_buffer.clear();
        processing_buffer.clear();
        while (images_buffer.size() < frame_rate)
            usleep(100);

        {
            const std::lock_guard<std::mutex> lock(img_queue_lock);
            size_t N = (images_buffer.size() > frame_rate) ? frame_rate : images_buffer.size();
            for (size_t i = 0; i < N; i++)
            {
                processing_buffer.push_back(images_buffer.front());
                images_buffer.pop();
            }
        }

        for (size_t i = 0; i < processing_buffer.size(); i++)
        {
            frame = processing_buffer[i];
            if (i == 0)
            {
                boxes = detector->detect(frame);

                count = 0;
                for (const auto& box : boxes)
                {
                    if (box.score < 0.95)
                        continue;

                    x1 = std::max(0, box.x1);
                    y1 = std::max(0, box.y1);
                    width = std::min(frame.cols - y1, box.y2 - box.y1 + 1);
                    height = std::min(frame.rows - x1, box.x2 - box.x1 + 1);
                    
                    cv::Ptr<cv::Tracker> tracker = cv::TrackerBoosting::create();
                    tracker->init(frame, cv::Rect2d(y1, x1, width, height));
                    trackers.push_back(tracker);

                    std::vector<cv::Mat> face_imgs;
                    face_imgs.push_back(cv::Mat(frame, cv::Rect(y1, x1, width, height)));
                    faces_buffer.insert(std::pair<int, std::vector<cv::Mat>>(count++, face_imgs));
                }
            }
            else
            {
                for (size_t j = 0; j < trackers.size(); j++)
                {
                    if (trackers[j]->update(frame, bbox))
                        faces_buffer[j].push_back(cv::Mat(frame, cv::Rect(bbox.x, bbox.y, bbox.width, bbox.height)));
                }
            }
        }

        for (size_t i = 0; i < trackers.size(); i++)
        {
            if (!faces_buffer[i].empty())
            {
                name = classifier->predict_block(mfn->get_embeddings(faces_buffer[i]));
                if (name == "unknown")
                    logger::LOG_INFO(DETECTED_UNKNOWN, "detected an unknown personal");
                else
                    logger::LOG_INFO(DETECTED_KNOWN, "detected " + name + " personal");
            }
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

    cv::Mat frame;
    while (true)
    {
        if (cap.read(frame))
        {
            cv::rotate(frame, frame, cv::ROTATE_90_CLOCKWISE);
            const std::lock_guard<std::mutex> lock(img_queue_lock);
            images_buffer.push(frame);
        }
        else
        {
            std::cout << "[CAMERA] frame droped!!!" << std::endl;
            continue;
        }

        if (cv::waitKey(1000/frame_rate) == 27)
            break;
    }
}
} // namespace tinyeye
