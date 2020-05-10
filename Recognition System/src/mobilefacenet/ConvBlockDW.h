#ifndef CONVBLOCKDW_H_H_
#define CONVBLOCKDW_H_H_

#include <torch/torch.h>

struct ConvBlockDWImpl : torch::nn::Module
{
    ConvBlockDWImpl(int input, int output, int kernel_size, int stride, int padding)
        : conv(torch::nn::Conv2dOptions(input, output, kernel_size).stride(stride).bias(false).padding(padding).groups(input)),
        batch_norm(torch::nn::BatchNorm2dOptions(output))
    {
        register_module("conv", conv);
        register_module("batch_norm", batch_norm);
    }

    torch::Tensor forward(torch::Tensor x)
    {
        x = conv(x);
        return batch_norm(x);
    }

    torch::nn::Conv2d conv;
    torch::nn::BatchNorm2d batch_norm;
};

TORCH_MODULE(ConvBlockDW);

#endif