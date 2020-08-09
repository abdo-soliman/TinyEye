#ifndef CONV_BLOCK_DW_H_H_
#define CONV_BLOCK_DW_H_H_

#include <torch/torch.h>

namespace tinyeye
{
namespace mobilefacenet
{
struct ConvBlockDWImpl : torch::nn::Module
{
    ConvBlockDWImpl(int inp, int oup, int k, int s, int p)
        :  conv(torch::nn::Conv2dOptions(inp, oup, k).stride(s).padding(p).groups(inp).bias(false)),
        bn(torch::nn::BatchNorm2dOptions(oup)),
        prelu(torch::nn::PReLUOptions().num_parameters(oup))
    {
        register_module("conv", conv);
        register_module("bn", bn);
        register_module("prelu", prelu);
    }

    torch::Tensor forward(torch::Tensor x)
    {
        x = conv(x);
        x = bn(x);
        return prelu(x);
    }

    torch::nn::Conv2d conv;
    torch::nn::BatchNorm2d bn;
    torch::nn::PReLU prelu;
};

TORCH_MODULE(ConvBlockDW);
} // namespace mobilefacenet
} // namespace tinyeye

#endif
