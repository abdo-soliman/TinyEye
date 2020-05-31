#ifndef TRAIN_H_H_
#define TRAIN_H_H_

#include "mobilefacenet.h"
#include "classifier/InnerProduct.h"
#include "classifier/ArcMarginProduct.h"
#include "dataloader/CASIA_WebFace_loader.h"
#include "config.h"
#include "utils.h"
#include <torch/torch.h>
#include <ctime>

using namespace mobile_facenet;

std::time_t train_time_now = std::time(0);
class train
{
private:
    /* data */
public:
    template <typename DataLoader>
    static void train_net(mobilefacenet &network, DataLoader &loader, torch::optim::Optimizer &optimizer, torch::optim::Optimizer &margin_optimizer, size_t epoch, size_t data_size, std::string classifier, InnerProduct &inner_margin, ArcMarginProduct &arc_margin)
    {
        size_t index = 0;
        network->train();
        if (classifier == "InnerProduct")
        {
            inner_margin->train();
        }
        else if (classifier == "ArcMarginProduct")
        {
            arc_margin->train();
        }
        else
        {
            inner_margin->train();
        }
        float Loss = 0, Acc = 0;
        auto criterion = torch::nn::CrossEntropyLoss();

        for (auto &batch : loader)
        {
            auto data = batch.data.to(config.device);
            auto targets = batch.target.to(config.device).view({-1});
            optimizer.zero_grad();
            margin_optimizer.zero_grad();

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
            else
            {
                margin_out = inner_margin->forward(output, targets);
            }

            auto loss = criterion(margin_out, targets);
            assert(!std::isnan(loss.template item<float>()));
            auto acc = margin_out.argmax(1).eq(targets).sum();

            // optimizer.zero_grad();
            loss.backward();
            optimizer.step();
            margin_optimizer.step();

            Loss += loss.template item<float>();
            Acc += acc.template item<float>();

            if (index++ % config.log_interval == 0)
            {
                auto end = std::min(data_size, (index + 1) * config.train_batch_size);

                std::cout << "Train Epoch: " << epoch << " " << end << "/" << data_size
                          << "\tLoss: " << Loss / end << "\tAcc: " << Acc / end
                          << std::endl;
                std::string file_name = std::string("../models/train-data-") + std::to_string(train_time_now) + ".csv";
                utils::write_in_csv(file_name, Loss / end, Acc / end, epoch);
            }
        }
    }
};

#endif
