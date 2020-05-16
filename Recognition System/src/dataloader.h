#ifndef DATALOADER_H_H_
#define DATALOADER_H_H_

#include <fstream>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <torch/torch.h>
#include <vector>
#include "config.h"

using Data = std::vector<std::pair<std::string, long>>;

std::pair<Data, Data> read_info(std::string input_file);

std::pair<Data, Data> read_info(std::string input_file)
{
    Data train, test;

    std::ifstream stream(input_file);
    assert(stream.is_open());

    long label;
    std::string path, type;

    while (true)
    {
        stream >> path >> label >> type;

        if (type == "train")
            train.push_back(std::make_pair(path, label));
        else if (type == "test")
            test.push_back(std::make_pair(path, label));
        else
            assert(false);

        if (stream.eof())
            break;
    }

    std::random_shuffle(train.begin(), train.end());
    std::random_shuffle(test.begin(), test.end());
    return std::make_pair(train, test);
}

class CustomDataset : public torch::data::datasets::Dataset<CustomDataset>
{
    using Example = torch::data::Example<>;

    Data data;

public:
    CustomDataset(const Data &data) : data(data) {}

    Example get(size_t index)
    {
        int image_size = config.image_size;
        std::string path = data[index].first;
        auto mat = cv::imread(path);
        mat.convertTo(mat, CV_32F, 1.0 / 255, 0);
        assert(!mat.empty());

        cv::resize(mat, mat, cv::Size(image_size, image_size));
        std::vector<cv::Mat> channels(3);
        cv::split(mat, channels);

        auto R = torch::from_blob(
            channels[2].ptr(),
            {image_size, image_size},
            torch::kUInt8);
        auto G = torch::from_blob(
            channels[1].ptr(),
            {image_size, image_size},
            torch::kUInt8);
        auto B = torch::from_blob(
            channels[0].ptr(),
            {image_size, image_size},
            torch::kUInt8);

        auto tdata = torch::cat({R, G, B})
                         .view({3, image_size, image_size})
                         .to(torch::kFloat);

        auto tlabel = torch::from_blob(&data[index].second, {1}, torch::kLong);
        return {tdata, tlabel};
    }

    torch::optional<size_t> size() const
    {
        return data.size();
    }
};

#endif
