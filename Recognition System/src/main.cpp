#include <iostream>
#include <opencv2/opencv.hpp>
#include "argparser/argparser.h"
#include <vector>
#include <ctime>
#include <torch/torch.h>
#include "dataloader.h"
#include "config.h"
#include "utils.h"
#include "trainmobilefacenet.h"
#include "testmobilefacenet.h"
#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/classifier/InnerProduct.h"
#include "mobilefacenet/classifier/ArcMarginProduct.h"
#include "mobilefacenet/classifier/CosineMarginProduct.h"
#include "argparser/cxxopts.hpp"

int main(int argc, char **argv)
{
    // ArgumentParser parser;
    // parser.add_option("--it", 'i', "16", "number of iterations or EPOCHs");
    // parser.add_option("--optimizer", 's', "Adam", "model optimizer (Adam, SGD)");
    // parser.add_option("--train-batch", 'i', "8", "train batch size");
    // parser.add_option("--test-batch", 'i', "200", "test batch size");
    // parser.add_option("--classifier", 's', "InnerProduct", "classifier (InnerProduct, ArcMarginProduct)");
    // parser.add_option("--easy_margin", 's', "false", "easy margin for ArcMarginProduct");

    // parser.parse(argc, argv);

    cxxopts::Options options("test", "A brief description");

    options.add_options()("iterations", "number of iterations or EPOCHs", cxxopts::value<int>()->default_value("16"))("optimizer", "model optimizer (Adam, SGD)", cxxopts::value<std::string>()->default_value("Adam"))("train-batch", "train batch size", cxxopts::value<int>()->default_value("8"))("test-batch", "test batch size", cxxopts::value<int>()->default_value("200"))("classifier", "classifier (InnerProduct, ArcMarginProduct, CosineMarginProduct)", cxxopts::value<std::string>()->default_value("InnerProduct"))("easy_margin", "easy margin for ArcMarginProduct", cxxopts::value<bool>()->default_value("false"))("h,help", "Print usage");

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

    std::time_t now = std::time(0);

    // parser.get_option("--it", iterations);
    // parser.get_option("--optimizer", optimizer);
    // parser.get_option("--train-batch", train_batch_size);
    // parser.get_option("--test-batch", test_batch_size);
    // parser.get_option("--classifier", classifier);
    // parser.get_option("--easy_margin", easy_margin);

    config.iterations = iterations;
    config.train_batch_size = train_batch_size;
    config.test_batch_size = test_batch_size;

    // save training parameters
    std::string file_name = std::string("../models/training-parameters-") + std::to_string(now) + ".txt";
    std::string training_data = std::string(optimizer) + " ," + std::string(classifier) + " ,train batch size " + std::to_string(train_batch_size) + " ,test batch size " + std::to_string(test_batch_size) + " ,iterations " + std::to_string(iterations) + " ,easy margin " + std::to_string(easy_margin);
    write_in_file(file_name, training_data);

    std::vector<std::string>
        faces_count = get_file_data("../data/faces_count.txt");

    torch::manual_seed(1);

    if (torch::cuda::is_available())
        config.device = torch::kCUDA;
    std::cout << "Running on: "
              << (config.device == torch::kCUDA ? "CUDA" : "CPU") << std::endl;

    auto data = read_info(config.infoFilePath);

    // std::cout << "Loading data ...\n";
    auto train_set =
        CustomDataset(data.first).map(torch::data::transforms::Stack<>());
    auto train_size = train_set.size().value();
    auto train_loader =
        torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            std::move(train_set), config.train_batch_size);

    auto test_set =
        CustomDataset(data.second).map(torch::data::transforms::Stack<>());
    auto test_size = test_set.size().value();
    auto test_loader =
        torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            std::move(test_set), config.test_batch_size);

    // std::cout << "constructing network ...\n";
    mobilefacenet network(config.features);
    network->to(config.device);
    // std::cout << "constructed network ...\n";

    InnerProduct inner_margin = InnerProduct(config.features, std::stoi(faces_count[0]));
    ArcMarginProduct arc_margin = ArcMarginProduct(config.features, std::stoi(faces_count[0]), easy_margin);
    CosineMarginProduct cos_margin = CosineMarginProduct(config.features, std::stoi(faces_count[0]));
    inner_margin->to(config.device);
    arc_margin->to(config.device);
    cos_margin->to(config.device);

    std::cout << faces_count[0] << "\n";

    // InnerProduct margin(config.features, is_int(faces_count[0]));

    // auto input = torch::Tensor(2, 3, 112, 112);
    // auto x = network(input);
    // std::cout << x << "\n";
    // torch::optim::SGD optimizer(
    //     {network->parameters(), margin->parameters()}, torch::optim::SGDOptions(0.01).momentum(0.5));
    torch::optim::Optimizer *generator_optimizer;
    if (optimizer == "Adam")
    {
        generator_optimizer = new torch::optim::Adam(
            network->parameters(), torch::optim::AdamOptions(2e-4));
    }
    else if (optimizer == "SGD")
    {
        generator_optimizer = new torch::optim::SGD(
            network->parameters(), torch::optim::SGDOptions(2e-4).momentum(0.5));
    }
    else
    {
        generator_optimizer = new torch::optim::Adam(
            network->parameters(), torch::optim::AdamOptions(2e-4));
    }

    // torch::optim::Adam discriminator_optimizer(
    //     discriminator->parameters(), torch::optim::AdamOptions(5e-4).beta1(0.5));
    // torch::optim::SGD generator_optimizer(
    //     network->parameters(), torch::optim::SGDOptions(2e-4).momentum(0.5));

    if (classifier == "InnerProduct")
    {
        generator_optimizer->add_parameters(inner_margin->parameters());
    }
    else if (classifier == "ArcMarginProduct")
    {
        generator_optimizer->add_parameters(arc_margin->parameters());
    }
    else if (classifier == "CosineMarginProduct")
    {
        generator_optimizer->add_parameters(cos_margin->parameters());
    }
    else
    {
        generator_optimizer->add_parameters(inner_margin->parameters());
    }
    // torch::optim::SGD discriminator_optimizer(
    //     margin->parameters(), torch::optim::SGDOptions(5e-4).momentum(0.5));

    for (size_t i = 0; i < config.iterations; ++i)
    {
        train(network, *train_loader, *generator_optimizer, i + 1, train_size, classifier, inner_margin, arc_margin, cos_margin);
        std::cout << std::endl;
        test(network, *test_loader, test_size, classifier, inner_margin, arc_margin, cos_margin);
        std::cout << std::endl;
    }

    torch::save(network, torch::str("../models/network-", now, ".pt"));
    if (classifier == "InnerProduct")
    {
        torch::save(inner_margin, torch::str("../models/inner_margin-", now, ".pt"));
    }
    else if (classifier == "ArcMarginProduct")
    {
        torch::save(arc_margin, torch::str("../models/arc_margin-", now, ".pt"));
    }
    else if (classifier == "CosineMarginProduct")
    {
        torch::save(cos_margin, torch::str("../models/cos_margin-", now, ".pt"));
    }
    else
    {
        torch::save(inner_margin, torch::str("../models/inner_margin-", now, ".pt"));
    }

    torch::save(*generator_optimizer, torch::str("../models/network-optimizer-", now, ".pt"));

    // std::vector<std::string> images_paths = get_file_data("../data/images.txt");
    // cv::Mat frame = cv::imread(images_paths[0]);
    // cv::imshow("Input Image", frame);
    // cv::waitKey(0);
    // frame.release();

    return 0;
}
