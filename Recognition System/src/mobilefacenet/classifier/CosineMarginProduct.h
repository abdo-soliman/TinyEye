#ifndef COSINEMARGINPRODUCT_H_H_
#define COSINEMARGINPRODUCT_H_H_

#include <torch/torch.h>
#include <math.h>

struct CosineMarginProductImpl : torch::nn::Module
{
    CosineMarginProductImpl(int in_feature = 128, int out_feature = 10575, double s = 32.0, double m = 0.50)
    {
        weight = register_parameter("weight", torch::randn({out_feature, in_feature}));
        // weight = torch::Parameter(torch::Tensor(out_feature, in_feature))
        torch::nn::init::xavier_uniform_(weight);
        this->_s = s;
        this->_m = m;
    }

    torch::Tensor forward(torch::Tensor x, torch::Tensor label)
    {
        auto cosine = torch::nn::functional::linear(torch::nn::functional::normalize(x), torch::nn::functional::normalize(weight));
        auto one_hot = torch::zeros_like(cosine);
        one_hot.scatter_(1, label.view({-1, 1}), 1.0);

        auto output = _s * (cosine - one_hot * _m);
        return output;
    }

    torch::Tensor weight;
    double _s;
    double _m;
};

TORCH_MODULE(CosineMarginProduct);

#endif