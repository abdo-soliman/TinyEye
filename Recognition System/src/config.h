#ifndef CONFIG_H_H_
#define CONFIG_H_H_

#include <iostream>
#include <torch/torch.h>

struct Config
{
    int image_size = 112;
    int features = 128;
    size_t train_batch_size = 8;
    size_t test_batch_size = 200;
    size_t iterations = 16;
    size_t log_interval = 20;
    // path must end in delimiter
    std::string infoFilePath = "../data/images.txt";
    torch::DeviceType device = torch::kCPU;
};

static Config config;

#endif