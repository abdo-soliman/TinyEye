#ifndef CONV_BLOCK_LINEAR_DW_H_H_
#define CONV_BLOCK_LINEAR_DW_H_H_

#include <utility>
#include <torch/torch.h>

namespace tinyeye
{
namespace mobilefacenet
{
struct ConvBlockLinearDWImpl : torch::nn::Module
{
    ConvBlockLinearDWImpl(int inp, int oup, std::pair<int, int> k, int s, int p)
        : conv(torch::nn::Conv2dOptions(inp, oup, {k.first, k.second}).stride(s).padding(p).groups(inp).bias(false)),
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

TORCH_MODULE(ConvBlockLinearDW);
} // namespace mobilefacenet
} // namespace tinyeye

#endif
