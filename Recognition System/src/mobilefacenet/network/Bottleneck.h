#ifndef BOTTLENECK_H_H_
#define BOTTLENECK_H_H_

#include <torch/torch.h>

namespace tinyeye
{
namespace mobilefacenet
{
struct BottleneckImpl : torch::nn::Module
{
    BottleneckImpl(int inp, int oup, int stride, int expansion)
        : conv(
            torch::nn::Conv2d(torch::nn::Conv2dOptions(inp, inp * expansion, 1).stride(1).padding(0).bias(false)),
            torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(inp * expansion)),
            torch::nn::PReLU(torch::nn::PReLUOptions().num_parameters(inp * expansion)),

            // dw
            torch::nn::Conv2d(torch::nn::Conv2dOptions(inp * expansion, inp * expansion, 3).stride(stride).padding(1).groups(inp * expansion).bias(false)),
            torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(inp * expansion)),
            torch::nn::PReLU(torch::nn::PReLUOptions().num_parameters(inp * expansion)),

            // pw-linear
            torch::nn::Conv2d(torch::nn::Conv2dOptions(inp * expansion, oup, 1).stride(1).padding(0).bias(false)),
            torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(oup))
        )
    {
        connect = (stride == 1) && (inp == oup);
        register_module("conv", conv);
    }

    torch::Tensor forward(torch::Tensor x)
    {
        if (connect)
            return x + conv->forward(x);
        else
            return conv->forward(x);
    }

    bool connect;
    torch::nn::Sequential conv;
};

TORCH_MODULE(Bottleneck);
} // namespace mobilefacenet
} // namespace tinyeye

#endif
