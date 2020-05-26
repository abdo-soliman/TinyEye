#ifndef MOBILEFACENET_H_H_
#define MOBILEFACENET_H_H_

#include <vector>
#include <torch/torch.h>

#include "layers/ConvBlock.h"
#include "layers/BottleNeck.h"
#include "layers/ConvBlockDW.h"
#include "layers/ConvBlockLinear.h"
#include "layers/ConvBlockLinearDW.h"

struct mobilefacenetImpl : torch::nn::Module
{
    mobilefacenetImpl(int feature_dim)
        : conv1(3, 64, 3, 2, 1),
          dw_conv1(64, 64, 3, 1, 1),
          conv2(128, 512, 1, 1, 0),
          linear7(512, 512, 7, 1, 0),
          linear1(512, feature_dim, 1, 1, 0)
    {
        blocks = make_layer();
        register_module("conv1", conv1);
        register_module("dw_conv1", dw_conv1);
        register_module("blocks", blocks);
        register_module("conv2", conv2);
        register_module("linear7", linear7);
        register_module("linear1", linear1);
    }

    torch::nn::Sequential make_layer()
    {
        int MobileFaceNet_BottleNeck_Setting[5][4] = {
            // t, c, n, s
            {2, 64, 5, 2},
            {4, 128, 1, 2},
            {2, 128, 6, 1},
            {4, 128, 1, 2},
            {2, 128, 2, 1}};
        torch::nn::Sequential layers;
        for (size_t i = 0; i < 4; i++)
        {
            int t = MobileFaceNet_BottleNeck_Setting[i][0];
            int c = MobileFaceNet_BottleNeck_Setting[i][1];
            int n = MobileFaceNet_BottleNeck_Setting[i][2];
            int s = MobileFaceNet_BottleNeck_Setting[i][3];
            for (int j = 0; j < n; j++)
            {
                if (j == 0)
                {
                    layers->push_back(BottleNeck(cur_channel, c, s, t));
                }
                else
                {
                    layers->push_back(BottleNeck(cur_channel, c, 1, t));
                }
                cur_channel = c;
            }
        }
        return layers;
    }

    ConvBlock conv1;
    ConvBlockDW dw_conv1;
    int cur_channel = 64;
    ConvBlock conv2;
    ConvBlockLinearDW linear7;
    ConvBlockLinear linear1;
    torch::nn::Sequential blocks;

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
};
TORCH_MODULE(mobilefacenet);

#endif