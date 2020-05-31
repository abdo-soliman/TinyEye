#ifndef CONVBLOCKLINEARDW_H_H_
#define CONVBLOCKLINEARDW_H_H_

#include <torch/torch.h>

namespace mobile_facenet
{
struct ConvBlockLinearDWImpl : torch::nn::Module
{
    ConvBlockLinearDWImpl(int input, int output, int kernel_size, int stride, int padding)
        : conv(torch::nn::Conv2dOptions(input, output, kernel_size).stride(stride).bias(false).padding(padding).groups(input)),
        batch_norm(torch::nn::BatchNorm2dOptions(output))
    {
        register_module("conv", conv);
        register_module("batch_norm", batch_norm);
    }

    torch::Tensor forward(torch::Tensor x)
    {
        x = conv(x);
        x = batch_norm(x);
        return x;
    }

    torch::nn::Conv2d conv;
    torch::nn::BatchNorm2d batch_norm;
};

TORCH_MODULE(ConvBlockLinearDW);
} // namespace mobile_facenet

#endif
