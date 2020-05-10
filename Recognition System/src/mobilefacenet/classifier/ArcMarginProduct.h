#ifndef ARCMARGINPRODUCT_H_H_
#define ARCMARGINPRODUCT_H_H_

#include <torch/torch.h>
#include <math.h>

struct ArcMarginProductImpl : torch::nn::Module
{
    ArcMarginProductImpl(int in_feature = 128, int out_feature = 10575, double s = 32.0, double m = 0.50, bool easy_margin = false)
    {
        weight = register_parameter("weight", torch::randn({out_feature, in_feature}));
        // weight = torch::Parameter(torch::Tensor(out_feature, in_feature))
        torch::nn::init::xavier_uniform_(weight);
        this->_easy_margin = easy_margin;
        this->_s = s;
        cos_m = cos(m);
        sin_m = sin(m);

        // # make the function cos(theta+m) monotonic decreasing while theta in [0°,180°]
        th = cos(M_PI - m);
        mm = sin(M_PI - m) * m;
    }

    torch::Tensor forward(torch::Tensor x, torch::Tensor label)
    {
        // x = torch::nn::functional::linear(x, weight);
        // return x;
        auto cosine = torch::nn::functional::linear(torch::nn::functional::normalize(x), torch::nn::functional::normalize(weight));
        // # cos(theta + m)
        auto sine = torch::sqrt(1.0 - torch::pow(cosine, 2));
        auto phi = cosine * cos_m - sine * sin_m;

        if (_easy_margin)
            phi = torch::where(cosine > 0, phi, cosine);
        else
            phi = torch::where((cosine - th) > 0, phi, cosine - mm);

        // #one_hot = torch.zeros(cosine.size(), device='cuda' if torch.cuda.is_available() else 'cpu')
        auto one_hot = torch::zeros_like(cosine);
        // std::cout << "in one_hot" << std::endl;
        one_hot.scatter_(1, label.view({-1, 1}), 1);
        // std::cout << "after one_hot" << std::endl;
        auto output = (one_hot * phi) + ((1.0 - one_hot) * cosine);
        output = output * _s;

        return output;
    }

    torch::Tensor weight;
    bool _easy_margin;
    double _s;
    double cos_m;
    double sin_m;
    double th;
    double mm;
};

TORCH_MODULE(ArcMarginProduct);

#endif