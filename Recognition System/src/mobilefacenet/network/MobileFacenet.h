#ifndef MOBILE_FACENET_H_H_
#define MOBILE_FACENET_H_H_

#include <utility>
#include <torch/torch.h>
#include "ConvBlock.h"
#include "ConvBlockDW.h"
#include "ConvBlockLinear.h"
#include "ConvBlockLinearDW.h"
#include "Bottleneck.h"

namespace tinyeye
{
namespace mobilefacenet
{
struct MobileFacenetImpl : torch::nn::Module
{
    MobileFacenetImpl()
        : conv1(3, 64, 3, 2, 1),
        dw_conv1(64, 64, 3, 1, 1),
        conv2(128, 512, 1, 1, 0),
        linear7(512, 512, std::make_pair(7, 6), 1, 0),
        linear1(512, 128, 1, 1, 0)
    {
        blocks = register_module("blocks", _make_layer());
        register_module("conv1", conv1);
        register_module("dw_conv1", dw_conv1);
        register_module("conv2", conv2);
        register_module("linear7", linear7);
        register_module("linear1", linear1);
    }

    torch::nn::Sequential _make_layer()
    {
        int Mobilefacenet_bottleneck_setting[5][4] = {
            // t, c, n, s
            {2, 64, 5, 2},
            {4, 128, 1, 2},
            {2, 128, 6, 1},
            {4, 128, 1, 2},
            {2, 128, 2, 1}
        };

        torch::nn::Sequential layers;
        for (size_t i = 0; i < 5; i++)
        {
            int t = Mobilefacenet_bottleneck_setting[i][0];
            int c = Mobilefacenet_bottleneck_setting[i][1];
            int n = Mobilefacenet_bottleneck_setting[i][2];
            int s = Mobilefacenet_bottleneck_setting[i][3];
            for (int j = 0; j < n; j++)
            {
                if (j == 0)
                    layers->push_back(Bottleneck(inplanes, c, s, t));
                else
                    layers->push_back(Bottleneck(inplanes, c, 1, t));
                inplanes = c;
            }
        }

        return layers;
    }

    torch::Tensor forward(torch::Tensor x)
    {
        x = conv1(x);
        x = dw_conv1(x);
        x = blocks->forward(x);
        x = conv2(x);
        x = linear7(x);
        x = linear1(x);
        x = x.view({x.size(0), -1});

        return x;
    }

    ConvBlock conv1;
    ConvBlockDW dw_conv1;
    int inplanes = 64;
    torch::nn::Sequential blocks;
    ConvBlock conv2;
    ConvBlockLinearDW linear7;
    ConvBlockLinear linear1;
};

TORCH_MODULE(MobileFacenet);
} // namespace mobilefacenet
} // namespace tinyeye

#endif
