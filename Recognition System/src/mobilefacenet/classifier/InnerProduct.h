#ifndef INNERPRODUCT_H_H_
#define INNERPRODUCT_H_H_

#include <torch/torch.h>

struct InnerProductImpl : torch::nn::Module
{
    InnerProductImpl(int in_feature = 128, int out_feature = 10575)
    {
        weight = register_parameter("weight", torch::randn({out_feature, in_feature}));
        // weight = torch::Parameter(torch::Tensor(out_feature, in_feature))
        torch::nn::init::xavier_uniform_(weight);
    }

    torch::Tensor forward(torch::Tensor x, torch::Tensor label)
    {
        x = torch::nn::functional::linear(x, weight);
        return x;
    }

    torch::Tensor weight;
};

TORCH_MODULE(InnerProduct);

#endif