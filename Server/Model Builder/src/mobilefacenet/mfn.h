#ifndef MFN_H_H_
#define MFN_H_H_

#include <memory>
#include <string>
#include <vector>
#include <torch/torch.h>
#include <opencv2/opencv.hpp>
#include "network/MobileFacenet.h"

namespace tinyeye
{
namespace mobilefacenet
{
class mfn
{
public:
    mfn(std::string model_path);
    torch::Tensor get_embeddings(torch::Tensor imgs);
    torch::Tensor get_embeddings(std::vector<cv::Mat>& imgs);
    torch::Tensor get_embeddings(cv::Mat& img);

private:
    MobileFacenet net;
};
} // namespace mobilefacenet
} // namespace tinyeye

#endif
