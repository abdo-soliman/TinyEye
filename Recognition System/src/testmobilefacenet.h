#ifndef TESTMOBILEFACENET_H_H_
#define TESTMOBILEFACENET_H_H_

#include "mobilefacenet/mobilefacenet.h"
#include "dataloader.h"
#include "config.h"
#include "utils.h"
#include <torch/torch.h>
#include "mobilefacenet/classifier/InnerProduct.h"
#include "mobilefacenet/classifier/ArcMarginProduct.h"
#include "mobilefacenet/classifier/CosineMarginProduct.h"
#include <ctime>

std::time_t test_time_now = std::time(0);
char *test_dt = std::ctime(&test_time_now);

template <typename DataLoader>
void test(mobilefacenet &network, DataLoader &loader, size_t data_size, std::string classifier, InnerProduct &inner_margin, ArcMarginProduct &arc_margin, CosineMarginProduct &cos_margin)
{
    size_t index = 0;
    network->eval();
    torch::NoGradGuard no_grad;
    float Loss = 0, Acc = 0;
    auto criterion = torch::nn::CrossEntropyLoss();

    for (const auto &batch : loader)
    {
        auto data = batch.data;
        auto targets = batch.target.view({-1});

        auto output = network->forward(data);
        torch::Tensor margin_out;

        if (classifier == "InnerProduct")
        {
            margin_out = inner_margin->forward(output, targets);
        }
        else if (classifier == "ArcMarginProduct")
        {
            margin_out = arc_margin->forward(output, targets);
        }
        else if (classifier == "CosineMarginProduct")
        {
            margin_out = cos_margin->forward(output, targets);
        }
        else
        {
            margin_out = inner_margin->forward(output, targets);
        }

        // auto loss = torch::nll_loss(output, targets);
        auto loss = criterion(margin_out, targets);
        assert(!std::isnan(loss.template item<float>()));
        auto acc = margin_out.argmax(1).eq(targets).sum();

        Loss += loss.template item<float>();
        Acc += acc.template item<float>();
    }

    if (index++ % config.log_interval == 0)
    {
        std::cout << "Test Loss: " << Loss / data_size
                  << "\tAcc: " << Acc / data_size << std::endl;
        std::string file_name = std::string("../models/test-data-") + std::to_string(test_time_now) + ".csv";
        write_in_csv(file_name, Loss / data_size, Acc / data_size, 1);
    }
}

#endif
