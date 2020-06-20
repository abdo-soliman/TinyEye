#include "img_loader.h"

#include <set>
#include <fstream>
#include <iostream>

#include "utils.h"
#include "mobilefacenet/config.h"

namespace tinyeye
{
namespace mobilefacenet
{
bool img_loader::get_face(mtcnn::MTCNN& detector, const cv::Mat& img, cv::Mat& face)
{
    std::vector<mtcnn::Bbox> boxes = detector.detect(img);

    if (boxes.empty())
        return false;

    int index = 0;
    float max_score = boxes[0].score;

    for (int i = 0; i < boxes.size(); i++)
    {
        if(boxes[i].score > max_score)
        {
            index = i;
            max_score = boxes[i].score;
        }
    }

    int x1 = std::max(0, boxes[index].x1);
    int y1 = std::max(0, boxes[index].y1);
    int width = std::min(img.cols - y1, boxes[index].y2 - boxes[index].y1 + 1);
    int height = std::min(img.rows - x1, boxes[index].x2 - boxes[index].x1 + 1);
    face = cv::Mat(img, cv::Rect(y1, x1, width, height));

    return true;
}

img_loader::img_loader(std::string maping_filename, std::string delimiter)
{
    std::ifstream input_file;
    input_file.open(maping_filename);
    std::vector<std::string> lines;
    if (input_file.is_open())
    {
        std::string line;
        while (getline(input_file, line))
            lines.push_back(line);
        
        utils::shuffle_vec(lines);
        std::vector<std::string> tokens;

        for (const auto& line : lines)
        {
            tokens = utils::split(line, delimiter);
            images_paths_list.push_back(tokens[0]);
            images_list.push_back(cv::imread(tokens[0]));

            if (!utils::is_uint(tokens[1]))
                throw std::invalid_argument("invalid label is not a an unsiged integer value");
            labels_list.push_back(std::stol(tokens[1].c_str()));
        }

        std::set<long> s(labels_list.begin(), labels_list.end());
        num_classes = s.size();
    }
    else
    {
        throw std::invalid_argument("can't open mapping file please make sure correct path is provided");
    }
}

std::vector<std::pair<WarningCode, std::string>> img_loader::use_mtcnn(mtcnn::mtcnn_config config)
{
    mtcnn::MTCNN detector(config);

    std::vector<std::pair<WarningCode, std::string>> warnings;
    std::vector<std::string> new_images_paths_list;
    std::vector<cv::Mat> new_images_list;
    std::vector<long> new_labels_list;
    
    cv::Mat face;
    for (size_t i = 0; i < images_list.size(); i++)
    {
        if (!get_face(detector, images_list[i], face))
        {
            warnings.push_back(std::make_pair(EMPTY_PHOTO, "Image " + images_paths_list[i] + " doesn't contain a face"));
            continue;
        }

        new_images_paths_list.push_back(images_paths_list[i]);
        new_images_list.push_back(face);
        new_labels_list.push_back(labels_list[i]);
    }

    std::set<long> s(new_labels_list.begin(), new_labels_list.end());
    if (s.size() != num_classes)
    {
        auto msg = std::to_string(num_classes-s.size()) + " classes have been dropped as their images doesn't contain faces";
        warnings.push_back(std::make_pair(CLASS_DROPED, msg));
    }

    images_list = new_images_list;
    labels_list = new_labels_list;
    num_classes = s.size();

    return warnings;
}

torch::data::Example<> img_loader::get(size_t index)
{
    auto tdata = img_to_tensor(images_list[index]);
    auto tlabel = torch::from_blob(&labels_list[index], {1}, torch::kLong).clone();

    return {tdata, tlabel};
}

torch::optional<size_t> img_loader::size() const
{
    return images_list.size();
}

long img_loader::get_num_classes()
{
    return num_classes;
}

torch::Tensor img_loader::img_to_tensor(cv::Mat img)
{
    assert(!img.empty() && img.channels() >= 2);
    cv::resize(img, img, cv::Size(config.image_width, config.image_height));

    img.convertTo(img, CV_32FC1);
    img = (img - 127.5) / 128.0;

    if (img.channels() == 2)
    {
        auto channel = torch::from_blob(
            img.ptr(),
            {config.image_height, config.image_width},
            torch::kFloat);

        return torch::cat({channel, channel, channel})
                            .view({3, config.image_height, config.image_width})
                            .to(torch::kFloat);
    }

    std::vector<cv::Mat> channels(3);
    cv::split(img, channels);

    auto R = torch::from_blob(
        channels[2].ptr(),
        {config.image_height, config.image_width},
        torch::kFloat);
    auto G = torch::from_blob(
        channels[1].ptr(),
        {config.image_height, config.image_width},
        torch::kFloat);
    auto B = torch::from_blob(
        channels[0].ptr(),
        {config.image_height, config.image_width},
        torch::kFloat);

    return torch::cat({B, G, R})
                        .view({3, config.image_height, config.image_width})
                        .to(torch::kFloat);
}
} // mobilefacenet
} // tinyeye
