#include "CASIA_WebFace_loader.h"
#include <ctime>
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "config.h"

namespace mobile_facenet
{
    CASIA_WebFace_loader::CASIA_WebFace_loader(std::string root_dir, std::string maping_filename)
    {
        std::ifstream input_file;
        std::vector<std::string> images_list;
        std::vector<long> labels_list;
        try
        {
            input_file.open(root_dir + "/" + maping_filename);
            if (input_file.is_open())
            {
                std::string image_path;
                long label;
                while (input_file >> image_path >> label)
                {
                    images_list.push_back(image_path);
                    labels_list.push_back(label);
                }

                num_classes = std::unique(labels_list.begin(), labels_list.end()) - labels_list.begin();
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

    torch::data::Example<> CASIA_WebFace_loader::get(size_t index)
    {
        cv::Mat img = cv::imread(images_list[index]);
        assert(!img.empty() && img.channels() >= 2);

        srand(clock());
        int direction = 0;
        if (((double)rand() / (RAND_MAX)) >= 0.5)
            direction = 1;
        else
            direction = -1;

        cv::flip(img, img, direction);
        img.convertTo(img, CV_32FC1);
        img = (img - 127.5) / 128.0;

        auto tlabel = torch::from_blob(&labels_list[index], {1}, torch::kLong);
        if (img.channels() == 2)
        {
            auto channel = torch::from_blob(
                img.ptr(),
                {config.image_height, config.image_width},
                torch::kFloat);

            auto tdata = torch::cat({channel, channel, channel})
                             .view({3, config.image_height, config.image_width})
                             .to(torch::kFloat);

            return {tdata, tlabel};
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

        auto tdata = torch::cat({R, G, B})
                         .view({3, config.image_height, config.image_width})
                         .to(torch::kFloat);

        return {tdata, tlabel};
    }

    torch::optional<size_t> CASIA_WebFace_loader::size() const
    {
        return images_list.size();
    }
} // namespace mobile_facenet
