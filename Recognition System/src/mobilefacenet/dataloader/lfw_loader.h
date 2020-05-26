#ifndef LFW_LOADER_H_H_
#define LFW_LOADER_H_H_

#include <vector>
#include <string>
#include <torch/torch.h>
#include <opencv2/opencv.hpp>

namespace mobile_facenet
{
class lfw_loader : public torch::data::datasets::Dataset<lfw_loader>
{
public:
    lfw_loader(std::string root_dir, std::string maping_filename);
    torch::Tensor image2tensor(cv::Mat img);
    torch::data::Example<> get(size_t index);
    torch::optional<size_t> size() const;

private:
    std::vector<std::string> imgl_list;
    std::vector<std::string> imgr_list;
};
} // namespace mobile_facenet

#endif
