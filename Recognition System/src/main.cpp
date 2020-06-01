#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <torch/script.h>
#include "config.h"
#include "utils.h"
#include "argparser/cxxopts.hpp"
#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/classifier/ArcMarginProduct.h"
#include "mobilefacenet/classifier/CosineMarginProduct.h"
#include "mobilefacenet/classifier/InnerProduct.h"
#include "mobilefacenet/lfw_evaluator.h"
#include "mobilefacenet/train.h"
#include "mobilefacenet/dataloader/lfw_loader.h"
#include "mobilefacenet/dataloader/CASIA_WebFace_loader.h"
using namespace mobile_facenet;

int main(int argc, char **argv)
{
    cxxopts::Options options("test", "A brief description");

    options.add_options()("iterations", "number of iterations or EPOCHs", cxxopts::value<int>()->default_value("16"))("optimizer", "model optimizer (Adam, SGD)", cxxopts::value<std::string>()->default_value("Adam"))("train-batch", "train batch size", cxxopts::value<int>()->default_value("8"))("test-batch", "test batch size", cxxopts::value<int>()->default_value("200"))("classifier", "classifier (InnerProduct, ArcMarginProduct)", cxxopts::value<std::string>()->default_value("InnerProduct"))("easy_margin", "easy margin for ArcMarginProduct", cxxopts::value<bool>()->default_value("false"))("resume", "resume training from last check point", cxxopts::value<bool>()->default_value("false"))("optimizer-check", "last checkpoint for optimizer", cxxopts::value<std::string>()->default_value(""))("model-check", "last checkpoint for model", cxxopts::value<std::string>()->default_value(""))("innermargin-check", "last checkpoint for innermargin", cxxopts::value<std::string>()->default_value(""))("arcmargin-check", "last checkpoint for arcmargin", cxxopts::value<std::string>()->default_value(""))("h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    std::string optimizer = result["optimizer"].as<std::string>();
    std::string classifier = result["classifier"].as<std::string>();
    size_t train_batch_size = result["train-batch"].as<int>();
    size_t test_batch_size = result["test-batch"].as<int>();
    size_t iterations = result["iterations"].as<int>();
    bool easy_margin = result["easy_margin"].as<bool>();
    bool resume = result["resume"].as<bool>();
    std::string optimizer_check = result["optimizer-check"].as<std::string>();
    std::string model_check = result["model-check"].as<std::string>();
    std::string innermargin_check = result["innermargin-check"].as<std::string>();
    std::string arcmargin_check = result["arcmargin-check"].as<std::string>();

    std::time_t now = std::time(0);

    config.iterations = iterations;
    config.train_batch_size = train_batch_size;
    config.test_batch_size = test_batch_size;

    // save training parameters
    std::string file_name = std::string("../models/training-parameters-") + std::to_string(now) + ".txt";
    std::string training_data = std::string(optimizer) + " ," + std::string(classifier) + " ,train batch size " + std::to_string(train_batch_size) + " ,test batch size " + std::to_string(test_batch_size) + " ,iterations " + std::to_string(iterations) + " ,easy margin " + std::to_string(easy_margin);
    utils::write_in_file(file_name, training_data);

    torch::manual_seed(now);

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
        // if (classifier == "InnerProduct")
        // {
        //     torch::load(inner_margin, innermargin_check);
        // }
        // else if (classifier == "ArcMarginProduct")
        // {
        //     torch::load(arc_margin, arcmargin_check);
        // }
        // else
        // {
        //     torch::load(inner_margin, innermargin_check);
        // }

        // torch::load(*generator_optimizer, optimizer_check);
    }

    network->to(config.device);
    for (size_t i = 0; i < config.iterations; ++i)
    {
        std::cout << "\nStart training...\n";
        train::train_net(network, *train_loader, *generator_optimizer, *margin_optimizer, i + 1, train_size, classifier, inner_margin, arc_margin);

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

    return 0;
}
