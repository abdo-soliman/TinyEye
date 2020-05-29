#include "lfw_evaluator.h"

#include <cmath>
#include <fstream>
#include <iostream>

#include "../utils.h"
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
                                std::vector<std::string> &nameLs, std::vector<std::string> &nameRs, std::vector<int> &flags)
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
                tokens = utils::split(line, "\t");
                if (tokens.size() == 3)
                {
                    nameLs.push_back(format(root, foldername, tokens[0], tokens[1]));
                    nameRs.push_back(format(root, foldername, tokens[0], tokens[2]));
                    flags.push_back(1);
                }
                else if (tokens.size() == 4)
                {
                    nameLs.push_back(format(root, foldername, tokens[0], tokens[1]));
                    nameRs.push_back(format(root, foldername, tokens[2], tokens[3]));
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

torch::Tensor lfw_evaluator::skip_fold(const torch::Tensor &src, int fold, int fold_size, bool vec)
{
    int num_rows = src.sizes()[0];
    torch::Tensor sub_tensor, before, after;
    if (fold == 0)
        sub_tensor = torch::narrow(src, 0, fold_size, num_rows - fold_size);
    else if (fold == num_rows / fold_size)
        sub_tensor = torch::narrow(src, 0, 0, num_rows - fold_size);
    else
    {
        int index = (fold + 1) * fold_size;
        before = torch::narrow(src, 0, 0, fold * fold_size);
        after = torch::narrow(src, 0, index, num_rows - index);
        if (vec)
            sub_tensor = torch::cat({before, after})
                             .view({num_rows - fold_size})
                             .to(torch::kFloat);
        else
            sub_tensor = torch::cat({before, after})
                             .view({num_rows - fold_size, config.features*2})
                             .to(torch::kFloat);
    }

    return sub_tensor;
}

float lfw_evaluator::get_accuracy(const torch::Tensor &scores, const std::vector<int> &flags, const float &threshold)
{
    std::vector<int64_t> positives;
    std::vector<int64_t> negatives;

    for (int i = 0; i < flags.size(); i++)
    {
        if (flags[i] == 1)
            positives.push_back(i);
        else
            negatives.push_back(i);
    }

    auto p_idx = torch::from_blob(positives.data(), {positives.size()}, torch::kLong);
    auto n_idx = torch::from_blob(negatives.data(), {negatives.size()}, torch::kLong);
    auto p_results = (torch::gather(scores, 0, p_idx) > threshold).to(torch::kFloat);
    auto n_results = (torch::gather(scores, 0, n_idx) < threshold).to(torch::kFloat);

    float p = *(torch::sum(p_results).data_ptr<float>());
    float n = *(torch::sum(n_results).data_ptr<float>());
    return 1.0 * (p + n) / scores.sizes()[0];
}

float lfw_evaluator::get_threshold(const torch::Tensor &scores, const std::vector<int> &flags, int thr_num)
{
    std::vector<float> thresholds;
    std::vector<float> accuracies;

    float max_acc = get_accuracy(scores, flags, -1);
    for (int i = -thr_num; i < thr_num + 1; i++)
    {
        float threshold = (i * 1.0) / thr_num;
        float acc = get_accuracy(scores, flags, threshold);
        if (acc > max_acc)
            max_acc = acc;

        thresholds.push_back(threshold);
        accuracies.push_back(acc);
    }

    float sum = 0;
    int count = 0;
    for (int i = 0; i < accuracies.size(); i++)
    {
        if (fabs(max_acc - accuracies[i]) < EPSILON)
        {
            sum += thresholds[i];
            count++;
        }
    }

    return sum / count;
}

void lfw_evaluator::get_val_flags(const std::vector<int>& flags, int fold, int fold_size, std::vector<int>& val_flags)
{
    val_flags.clear();
    for (int i = 0; i < flags.size(); i++)
    {
        if (i == fold*fold_size)
        {
            for (int j = 0; j < fold_size; j++)
                i++;
        }

        if (i < flags.size())
            val_flags.push_back(flags[i]);
    }
}

void lfw_evaluator::get_test_flags(const std::vector<int>& flags, int fold, int fold_size, std::vector<int>& test_flags)
{
    test_flags.clear();
    int index = fold*fold_size;
    for (int i = 0; i < fold_size; i++)
        test_flags.push_back(flags[index++]);
}

std::vector<float> lfw_evaluator::evaluation_10_fold(const torch::Tensor& left_features, const torch::Tensor& right_features,
                                                     const std::vector<int>& flags)
{
    
    std::vector<float> accs;
    std::vector<int> val_flags;
    std::vector<int> test_flags;

    for (int i = 0; i < 10; i++)
    {
        auto left_sub_tensor = skip_fold(left_features, i, FOLD_SIZE);
        auto right_sub_tensor = skip_fold(right_features, i, FOLD_SIZE);
        int left_size = left_sub_tensor.sizes()[0];
        int right_size = right_sub_tensor.sizes()[0];

        auto contact = torch::cat({left_sub_tensor, right_sub_tensor})
                           .view({left_size + right_size, config.features*2})
                           .to(torch::kFloat);

        auto mu = torch::mean(contact, 0).view({1, config.features*2});
        auto norm_left = left_features - mu;
        auto norm_right = right_features - mu;

        norm_left = norm_left / torch::sqrt(torch::sum(norm_left * norm_left, 1)).view({norm_left.sizes()[0], 1});
        norm_right = norm_right / torch::sqrt(torch::sum(norm_right * norm_right, 1)).view({norm_right.sizes()[0], 1});
        auto scores = torch::sum(norm_left * norm_right, 1);

        get_val_flags(flags, i, FOLD_SIZE, val_flags);
        get_test_flags(flags, i, FOLD_SIZE, test_flags);

        float threshold = get_threshold(skip_fold(scores, i, FOLD_SIZE, true), val_flags, 10000);
        accs.push_back(get_accuracy(torch::narrow(scores, 0, i*FOLD_SIZE, FOLD_SIZE), test_flags, threshold));
    }

    return accs;
}

void lfw_evaluator::evaluate(mobilefacenet& network, lfw_loader& dataset, const std::vector<int>& flags)
{
    network->eval();
    auto loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
        std::move(dataset), config.test_batch_size);

    torch::Tensor left_list[6000];
    torch::Tensor right_list[6000];
    int index = 0;
	size_t count = 0;
    for (const auto &batch : *loader)
    {
		count += config.test_batch_size;
		std::cout << "extracing deep features from the face pair " << count << "...\n";
        for (const auto &sample : batch)
        {
            auto left_imgs = sample.data.to(config.device);
            auto right_imgs = sample.target.to(config.device);
            left_list[index] = (network->forward(left_imgs)).view({1, config.features*2});
            right_list[index] = (network->forward(right_imgs)).view({1, config.features*2});
            index++;
        }
    }

    auto left_features = torch::cat({left_list}).view({6000, config.features*2});
    auto right_features = torch::cat({right_list}).view({6000, config.features*2});
    std::vector<float> accs = evaluation_10_fold(left_features, right_features, flags);

    float sum = 0;
    for (int i = 0; i < 10; i++)
    {
        sum += accs[i];
        std::cout << i + 1 << '\t' << accs[i]*100.0 << '\n';
    }

    std::cout << "--------\n";
    std::cout << "AVE\t" << (sum / 10.0) * 100.0 << "\n";
}
} // namespace mobile_facenet
