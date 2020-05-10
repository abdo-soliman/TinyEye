#ifndef MARGIN_H_H_
#define MARGIN_H_H_

#include <torch/torch.h>

class MarginImpl : public torch::nn::Module
{
public:
    virtual torch::Tensor forward(torch::Tensor x, torch::Tensor label);
};

TORCH_MODULE(Margin);

#endif