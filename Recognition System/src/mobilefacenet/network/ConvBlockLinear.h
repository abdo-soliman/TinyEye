#ifndef CONV_BLOCK_LINEAR_H_H_
#define CONV_BLOCK_LINEAR_H_H_

#include <torch/torch.h>

namespace tinyeye
{
namespace mobilefacenet
{
struct ConvBlockLinearImpl : torch::nn::Module
{
    ConvBlockLinearImpl(int inp, int oup, int k, int s, int p)
        : conv(torch::nn::Conv2dOptions(inp, oup, k).stride(s).padding(p).bias(false)),
        bn(torch::nn::BatchNorm2dOptions(oup))
    {
        register_module("conv", conv);
        register_module("bn", bn);
    }

    torch::Tensor forward(torch::Tensor x)
    {
        x = conv(x);
        return bn(x);
    }

    torch::nn::Conv2d conv;
    torch::nn::BatchNorm2d bn;
};

TORCH_MODULE(ConvBlockLinear);
} // namespace mobilefacenet
} // namespace tinyeye

#endif
