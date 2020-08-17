#ifndef IMG_LOADER_H_H_
#define IMG_LOADER_H_H_

#include <vector>
#include <string>
#include <torch/torch.h>
#include <opencv2/opencv.hpp>

namespace tinyeye
{
namespace mobilefacenet
{
class img_loader : public torch::data::datasets::Dataset<img_loader>
{
public:
    img_loader(std::string maping_filename);
    torch::data::Example<> get(size_t index);
    torch::optional<size_t> size() const;
    long get_num_classes();

    static torch::Tensor img_to_tensor(cv::Mat img);

private:
    std::vector<std::string> images_list;
    std::vector<long> labels_list;
    long num_classes;
};
} // mobilefacenet
} // tinyeye

#endif
