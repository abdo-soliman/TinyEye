#ifndef CONFIG_H_H_
#define CONFIG_H_H_

#include <torch/torch.h>

namespace tinyeye
{
namespace mobilefacenet
{
struct Config
{
    int image_width = 96;
    int image_height = 112;
    int features = 128;
    size_t train_batch_size = 32;
    size_t test_batch_size = 32;
    size_t iterations = 30;
    torch::DeviceType device = torch::kCPU;
};

static Config config;
} // mobilefacenet
} // tinyeye

#endif
