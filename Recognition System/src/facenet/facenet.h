#ifndef FACENET_H_H_
#define FACENET_H_H_

#include <string>
#include <opencv2/opencv.hpp>

class Facenet
{
private:
    void normalize_faces(std::vector<cv::Mat> &faces);
public:
    Facenet(std::string model_path);
    void get_embeddings(std::vector<cv::Mat> &faces);
};

#endif
