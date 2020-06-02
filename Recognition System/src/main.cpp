#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include "config.h"
#include "argparser/cxxopts.hpp"
#include "mobilefacenet/train.h"
using namespace mobile_facenet;

int main(int argc, char **argv)
{
    cxxopts::Options options("test", "A brief description");

    options.add_options()("iterations", "number of iterations or EPOCHs", cxxopts::value<int>()->default_value("16"))("optimizer", "model optimizer (Adam, SGD)", cxxopts::value<std::string>()->default_value("Adam"))("train-batch", "train batch size", cxxopts::value<int>()->default_value("8"))("test-batch", "test batch size", cxxopts::value<int>()->default_value("200"))("classifier", "classifier (InnerProduct, ArcMarginProduct)", cxxopts::value<std::string>()->default_value("InnerProduct"))("easy_margin", "easy margin for ArcMarginProduct", cxxopts::value<bool>()->default_value("false"))("resume", "resume training from last check point", cxxopts::value<bool>()->default_value("false"))("optimizer-check", "last checkpoint for optimizer", cxxopts::value<std::string>()->default_value(""))("model-check", "last checkpoint for model", cxxopts::value<std::string>()->default_value(""))("innermargin-check", "last checkpoint for innermargin", cxxopts::value<std::string>()->default_value(""))("arcmargin-check", "last checkpoint for arcmargin", cxxopts::value<std::string>()->default_value(""))("train", "train network", cxxopts::value<bool>()->default_value("false"))("h,help", "Print usage");

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
    bool train = result["train"].as<bool>();

    if (train == true)
    {
        std::cout << "Start training: " << train << std::endl;
        train::train_net(optimizer, classifier, train_batch_size, test_batch_size, iterations, easy_margin, resume, optimizer_check, model_check, innermargin_check, arcmargin_check);
    }else {
        std::cout << "No training: " << train << std::endl;
    }
    

    return 0;
}
