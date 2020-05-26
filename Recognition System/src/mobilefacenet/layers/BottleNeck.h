#ifndef BOTTLENECK_H_H_
#define BOTTLENECK_H_H_

#include <vector>
#include <torch/torch.h>

#include "config.h"

struct BottleNeckImpl : torch::nn::Module
{
    torch::nn::Sequential conv;
    bool connect;

    BottleNeckImpl(int input, int output, int stride, int expansion)
        : conv(
              torch::nn::Conv2d(torch::nn::Conv2dOptions(input, input * expansion, 1).stride(1).padding(0).bias(false)),
              torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(input * expansion)),
              torch::nn::PReLU(torch::nn::PReLUOptions().num_parameters(input * expansion)),
              // 3 * 3 depth wise conv
              torch::nn::Conv2d(torch::nn::Conv2dOptions(input * expansion, input * expansion, 3).stride(stride).padding(1).groups(input * expansion).bias(false)),
              torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(input * expansion)),
              torch::nn::PReLU(torch::nn::PReLUOptions().num_parameters(input * expansion)),
              // 1 * 1 conv
              torch::nn::Conv2d(torch::nn::Conv2dOptions(input * expansion, output, 1).stride(1).padding(0).bias(false)),
              torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(output)))
    {
        connect = stride == 1 and input == output;
        conv->to(config.device);
        // register_module("conv", conv);
    }

    torch::Tensor forward(torch::Tensor x)
    {
        if (connect)
        {
            x = x + conv->forward(x);
            return x;
        }
        x = conv->forward(x);
        return x;
    }
    // BottleNeckConv conv;
};

TORCH_MODULE(BottleNeck);

#endif