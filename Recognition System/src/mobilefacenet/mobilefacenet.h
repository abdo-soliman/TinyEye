#ifndef MOBILE_FACENET_H_H_
#define MOBILE_FACENET_H_H_

#include <memory>
#include <string>
#include <vector>
#include <torch/torch.h>
#include <torch/script.h>
#include <opencv2/opencv.hpp>

namespace tinyeye
{
namespace mobilefacenet
{
class MobileFacenet
{
public:
    MobileFacenet(std::string model_path);
    torch::Tensor get_embeddings(torch::Tensor imgs);
    torch::Tensor get_embeddings(std::vector<cv::Mat>& imgs);
    torch::Tensor get_embeddings(cv::Mat& img);

private:
    torch::jit::script::Module net;
};
} // mobilefacenet
} // tinyeye

#endif
