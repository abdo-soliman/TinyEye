#ifndef IMG_LOADER_H_H_
#define IMG_LOADER_H_H_

#include <vector>
#include <string>
#include <torch/torch.h>
#include <opencv2/opencv.hpp>

#include "logger/logger.h"
#include "mtcnn/mtcnn.h"

namespace tinyeye
{
namespace mobilefacenet
{
class img_loader : public torch::data::datasets::Dataset<img_loader>
{
public:
    img_loader(std::string maping_filename);
    std::vector<std::pair<WarningCode, std::string>> use_mtcnn(mtcnn::mtcnn_config config);
    torch::data::Example<> get(size_t index);
    torch::optional<size_t> size() const;
    long get_num_classes();

    static torch::Tensor img_to_tensor(cv::Mat img);

private:
    bool get_face(mtcnn::MTCNN& detector, const cv::Mat& img, cv::Mat& face);

    std::vector<std::string> images_paths_list;
    std::vector<cv::Mat> images_list;
    std::vector<long> labels_list;
    long num_classes;
};
} // mobilefacenet
} // tinyeye

#endif
