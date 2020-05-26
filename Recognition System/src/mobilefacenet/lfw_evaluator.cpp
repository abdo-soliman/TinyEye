#include "lfw_evaluator.h"

#include <fstream>
#include <iostream>
#include <torch/torch.h>

#include "utils.h"
#include "config.h"
#include "dataloader/lfw_loader.h"


namespace mobile_facenet
{
std::string lfw_evaluator::format(const std::string &root, const std::string &foldername,
                                  const std::string &filename, const std::string &number)
{
    std::string formated_number = std::string(4 - number.size(), '0') + number;

    return root + '/' + foldername + '/' + filename + '/' + filename + '_' + formated_number + ".jpg";
}

void lfw_evaluator::parse_pairs(const std::string &root, const std::string &foldername, const std::string &pairs_filename,
                                std::vector<std::string> &nameLs, std::vector<std::string> &nameRs, std::vector<int> &folds,
                                std::vector<int> &flags)
{
    std::ifstream pairs_file;
    try
    {
        pairs_file.open(root + "/" + pairs_filename);
        if (pairs_file.is_open())
        {
            std::string line;
            std::vector<std::string> tokens;

            std::getline(pairs_file, line); // skipping first line

            int i = 0;
            while (std::getline(pairs_file, line))
            {
                tokens = split(line, "\t");
                if (tokens.size() == 3)
                {
                    nameLs.push_back(format(root, foldername, tokens[0], tokens[1]));
                    nameRs.push_back(format(root, foldername, tokens[0], tokens[2]));
                    folds.push_back(i / 600);
                    flags.push_back(1);
                }
                else if (tokens.size() == 4)
                {
                    nameLs.push_back(format(root, foldername, tokens[0], tokens[1]));
                    nameRs.push_back(format(root, foldername, tokens[2], tokens[3]));
                    folds.push_back(i / 600);
                    flags.push_back(-1);
                }
                else
                {
                    i++;
                    continue;
                }

                i++;
            }
        }
        else
        {
            std::cerr << "Error: can't read from pairs file please make sure correct file path is provided\n";
            exit(-1);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        exit(-2);
    }
}

void lfw_evaluator::evaluate(mobilefacenet& network, std::string root, std::string foldername,
                             std::string pairs_filename, std::string feature_save_dir)
{
    network->eval();

    std::vector<std::string> imgl_list;
    std::vector<std::string> imgr_list;
    std::vector<int> folds;
    std::vector<int> flags;

    parse_pairs(root, foldername, pairs_filename, imgl_list, imgr_list, folds, flags);

    lfw_loader dataset = lfw_loader(imgl_list, imgr_list);
    auto loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
        std::move(dataset), config.test_batch_size);

    for (const auto &batch : *loader)
    {
        for (const auto &sample : batch)
        {
            auto left_imgs = sample.data.to(config.device);
            auto right_imgs = sample.target.to(config.device);
            auto left_embeddings = network->forward(left_imgs);
            auto right_embeddings = network->forward(right_imgs);
            std::cout << left_embeddings << '\n';
            break;
        }
        break;
    }
}
} // namespace mobile_facenet
