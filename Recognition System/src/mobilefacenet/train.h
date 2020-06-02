#ifndef TRAIN_H_H_
#define TRAIN_H_H_

#include "classifier/InnerProduct.h"
#include "classifier/ArcMarginProduct.h"
#include "dataloader/CASIA_WebFace_loader.h"
#include "config.h"
#include "utils.h"
#include <torch/torch.h>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <torch/script.h>
#include "mobilefacenet.h"
#include "lfw_evaluator.h"
#include "dataloader/lfw_loader.h"

using namespace mobile_facenet;

std::time_t train_time_now = std::time(0);
class train
{
private:
    /* data */
    static void save_training_parameters(std::string optimizer, std::string classifier, size_t train_batch_size, size_t test_batch_size, size_t iterations, bool easy_margin, std::time_t now){
        std::string file_name = std::string("../models/training-parameters-") + std::to_string(now) + ".txt";
        std::string training_data = std::string(optimizer) + " ," + std::string(classifier) + " ,train batch size " + std::to_string(train_batch_size) + " ,test batch size " + std::to_string(test_batch_size) + " ,iterations " + std::to_string(iterations) + " ,easy margin " + std::to_string(easy_margin);
        utils::write_in_file(file_name, training_data);
    }
public:
    static void train_net(std::string optimizer, std::string classifier, size_t train_batch_size, size_t test_batch_size, size_t iterations, bool easy_margin, bool resume, std::string optimizer_check, std::string model_check, std::string innermargin_check, std::string arcmargin_check) {
        std::time_t now = std::time(0);
        config.iterations = iterations;
        config.train_batch_size = train_batch_size;
        config.test_batch_size = test_batch_size;

        torch::manual_seed(now);

        save_training_parameters(optimizer, classifier, config.train_batch_size, config.test_batch_size, iterations, easy_margin, now);

        if (torch::cuda::is_available())
            config.device = torch::kCUDA;
        std::cout << "Running on: "
                  << (config.device == torch::kCUDA ? "CUDA" : "CPU") << std::endl;

        std::vector<int> flags;
        std::vector<std::string> imgl_list;
        std::vector<std::string> imgr_list;

        std::vector<std::string>
            faces_count = utils::get_file_data("../data/faces_count.txt");

        std::cout << "num of faces " << faces_count[0] << "\n";


        std::string lfw_root = "../data";
        lfw_evaluator::parse_pairs(lfw_root, "lfw-96x112", "pairs.txt", imgl_list, imgr_list, flags);

        auto test_dataset = lfw_loader(imgl_list, imgr_list).map(torch::data::transforms::Stack<>());
        auto test_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            std::move(test_dataset), config.test_batch_size);
        auto train_dataset = CASIA_WebFace_loader("../data/images.txt").map(torch::data::transforms::Stack<>());
        auto train_size = train_dataset.size().value();
        auto train_loader =
            torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
                std::move(train_dataset), config.train_batch_size);
        std::cout << "train data " << train_size << std::endl;

        mobilefacenet network(config.features);

        InnerProduct inner_margin = InnerProduct(config.features, std::stoi(faces_count[0]));
        ArcMarginProduct arc_margin = ArcMarginProduct(config.features, std::stoi(faces_count[0]), easy_margin);
        inner_margin->to(config.device);
        arc_margin->to(config.device);

        torch::optim::Optimizer *generator_optimizer;
        torch::optim::Optimizer *margin_optimizer;
        if (optimizer == "Adam")
        {
            generator_optimizer = new torch::optim::Adam(
                network->parameters(), torch::optim::AdamOptions(2e-4));
        }
        else if (optimizer == "SGD")
        {
            generator_optimizer = new torch::optim::SGD(
                network->parameters(), torch::optim::SGDOptions(0.1).momentum(0.5));
        }
        else
        {
            generator_optimizer = new torch::optim::Adam(
                network->parameters(), torch::optim::AdamOptions(2e-4));
        }

        if (classifier == "InnerProduct")
        {
            margin_optimizer = new torch::optim::SGD(
                inner_margin->parameters(), torch::optim::SGDOptions(0.1).momentum(0.5));
            // generator_optimizer->add_parameters(inner_margin->parameters());
        }
        else if (classifier == "ArcMarginProduct")
        {
            margin_optimizer = new torch::optim::SGD(
                arc_margin->parameters(), torch::optim::SGDOptions(0.1).momentum(0.5));
            // generator_optimizer->add_parameters(arc_margin->parameters());
        }
        else
        {
            margin_optimizer = new torch::optim::SGD(
                inner_margin->parameters(), torch::optim::SGDOptions(0.1).momentum(0.5));
            // generator_optimizer->add_parameters(inner_margin->parameters());
        }

        if (resume)
        {
            std::cout << "resuming..." << std::endl;
            torch::load(network, model_check);
            if (classifier == "InnerProduct")
            {
                torch::load(inner_margin, innermargin_check);
            }
            else if (classifier == "ArcMarginProduct")
            {
                torch::load(arc_margin, arcmargin_check);
            }
            else
            {
                torch::load(inner_margin, innermargin_check);
            }

            torch::load(*generator_optimizer, optimizer_check);
        }

        network->to(config.device);
        for (size_t i = 0; i < config.iterations; ++i)
        {
            std::cout << "\nStart training...\n";
            train_loop(network, *train_loader, *generator_optimizer, *margin_optimizer, i + 1, train_size, classifier, inner_margin, arc_margin);

            std::cout << "\nStart testing on lfw\n";
            lfw_evaluator::evaluate(network, test_loader, flags);

            std::cout << "\nSaving checkpoint...\n";
            torch::save(network, torch::str("../models/network-", now, ".pt"));
            if (classifier == "InnerProduct")
            {
                torch::save(inner_margin, torch::str("../models/inner_margin-", now, ".pt"));
            }
            else if (classifier == "ArcMarginProduct")
            {
                torch::save(arc_margin, torch::str("../models/arc_margin-", now, ".pt"));
            }
            else
            {
                torch::save(inner_margin, torch::str("../models/inner_margin-", now, ".pt"));
            }

            torch::save(*generator_optimizer, torch::str("../models/network-optimizer-", now, ".pt"));
        }
    }
    template <typename DataLoader>
    static void train_loop(mobilefacenet &network, DataLoader &loader, torch::optim::Optimizer &optimizer, torch::optim::Optimizer &margin_optimizer, size_t epoch, size_t data_size, std::string classifier, InnerProduct &inner_margin, ArcMarginProduct &arc_margin)
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
