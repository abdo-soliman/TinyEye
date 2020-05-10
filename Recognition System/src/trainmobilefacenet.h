#ifndef TRAINMOBILEFACENET_H_H_
#define TRAINMOBILEFACENET_H_H_

#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/classifier/InnerProduct.h"
#include "mobilefacenet/classifier/ArcMarginProduct.h"
#include "dataloader.h"
#include "config.h"
#include "utils.h"
#include <torch/torch.h>
#include <ctime>

std::time_t train_time_now = std::time(0);
char *train_dt = std::ctime(&train_time_now);

template <typename DataLoader>
void train(
    mobilefacenet &network,
    DataLoader &loader,
    torch::optim::Optimizer &optimizer,
    // torch::optim::Optimizer &discriminator_optimizer,
    size_t epoch,
    size_t data_size,
    std::string classifier,
    InnerProduct &inner_margin,
    ArcMarginProduct &arc_margin)
{
    size_t index = 0;
    network->train();
    float Loss = 0, Acc = 0;
    auto criterion = torch::nn::CrossEntropyLoss();

    for (auto &batch : loader)
    {
        auto data = batch.data;
        // std::cout << data << std::endl;
        auto targets = batch.target.view({-1});
        // std::cout << targets << std::endl;
        optimizer.zero_grad();
        // discriminator_optimizer.zero_grad();

        auto output = network->forward(data);
        // std::cout << typeid(targets).name() << '\n';

        // auto margin_out = inner_margin->forward(output, targets);
        torch::Tensor margin_out;

        if (classifier == "InnerProduct")
        {
            margin_out = inner_margin->forward(output, targets);
        }
        else if (classifier == "ArcMarginProduct")
        {

            margin_out = arc_margin->forward(output, targets);
        }
        else
        {
            margin_out = inner_margin->forward(output, targets);
        }

        // std::cout << targets << std::endl;
        // std::cout << margin_out << std::endl;

        auto loss = criterion(margin_out, targets);
        // auto loss = torch::nll_loss(torch::log_softmax(margin_out, /*dim=*/1), targets); //torch::nll_loss(margin_out, targets);
        assert(!std::isnan(loss.template item<float>()));
        auto acc = margin_out.argmax(1).eq(targets).sum();

        optimizer.zero_grad();
        // discriminator_optimizer.zero_grad();
        loss.backward();
        optimizer.step();
        // discriminator_optimizer.step();

        Loss += loss.template item<float>();
        Acc += acc.template item<float>();

        if (index++ % config.log_interval == 0)
        {
            auto end = std::min(data_size, (index + 1) * config.train_batch_size);

            std::cout << "Train Epoch: " << epoch << " " << end << "/" << data_size
                      << "\tLoss: " << Loss / end << "\tAcc: " << Acc / end
                      << std::endl;
            std::string file_name = std::string("../models/train-data-") + std::to_string(train_time_now) + ".csv";
            write_in_csv(file_name, Loss / end, Acc / end, epoch);
        }
    }
}

#endif
