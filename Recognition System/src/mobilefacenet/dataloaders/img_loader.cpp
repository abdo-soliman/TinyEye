#include "img_loader.h"

#include <fstream>
#include <iostream>

#include "mobilefacenet/config.h"

namespace tinyeye
{
namespace mobilefacenet
{
img_loader::img_loader(std::string maping_filename)
{
    std::ifstream input_file;
    try
    {
        input_file.open(maping_filename);
        if (input_file.is_open())
        {
            std::string image_path;
            long label;
            num_classes = 0;
            long current_class = -1;
            while (input_file >> image_path >> label)
            {
                if (current_class != label)
                {
                    num_classes++;
                    current_class = label;
                }

                images_list.push_back(image_path);
                labels_list.push_back(label);
            }
        }
        else
        {
            std::cerr << "Error: can't open file please make sure input_file path is correct\n";
            exit(-2);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        exit(-3);
    }
}

torch::data::Example<> img_loader::get(size_t index)
{
    cv::Mat img = cv::imread(images_list[index]);
    auto tdata = img_to_tensor(img);
    auto tlabel = torch::from_blob(&labels_list[index], {1}, torch::kLong);

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
