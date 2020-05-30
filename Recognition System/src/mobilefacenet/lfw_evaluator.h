#ifndef LFW_EVALUATOR_H_H_
#define LFW_EVALUATOR_H_H_

#include <string>
#include <vector>
#include <torch/torch.h>

#include "mobilefacenet.h"
#include "utils.h"
#include "dataloader/lfw_loader.h"

namespace mobile_facenet
{
    class lfw_evaluator
    {
    public:
        static void parse_pairs(std::string root, std::string foldername, std::string pairs_filename,
                                std::vector<std::string> &nameLs, std::vector<std::string> &nameRs, std::vector<int> &flags);
        static std::vector<float> evaluation_10_fold(const torch::Tensor &left_features, const torch::Tensor &right_features,
                                                     const std::vector<int> &flags);
        template <typename DataLoader>
        static void evaluate(mobilefacenet &network, DataLoader &loader, const std::vector<int> &flags)
        {
            network->eval();

            size_t count = 0;

            torch::Tensor left_features, right_features;
            std::vector<torch::Tensor> left_tensors, right_tensors;
            utils::write_in_file("../models/left_tensors.txt", "");
            utils::write_in_file("../models/right_tensors.txt", "");
            for (const auto &batch : *loader)
            {
                count += config.test_batch_size;
                std::cout << "extracing deep features from the face pair " << count << "...\n";
                auto left_batch = batch.data.to(config.device);
                auto right_batch = batch.target.to(config.device);
                left_batch = left_batch.view({2 * config.test_batch_size, 3, config.image_height, config.image_width});
                right_batch = right_batch.view({2 * config.test_batch_size, 3, config.image_height, config.image_width});
                auto left_batch_output = (network->forward(left_batch)).view({config.test_batch_size, config.features * 2}).cpu();
                auto right_batch_output = (network->forward(right_batch)).view({config.test_batch_size, config.features * 2}).cpu();
                for (int it = 0; it < config.test_batch_size; it++)
                {
                    utils::append_tensors("../models/left_tensors.txt", left_batch_output[it]);
                    utils::append_tensors("../models/right_tensors.txt", right_batch_output[it]);
                }
            }

            left_features = utils::get_tensors("../models/left_tensors.txt");
            right_features = utils::get_tensors("../models/right_tensors.txt");
            left_features = left_features.view({count, 2*config.features});
            right_features = right_features.view({count, 2*config.features});

            std::vector<float> accs = evaluation_10_fold(left_features, right_features, flags);

            float sum = 0;
            for (int i = 0; i < 10; i++)
            {
                sum += accs[i];
                std::cout << i + 1 << '\t' << accs[i] * 100.0 << '\n';
            }

            std::cout << "--------\n";
            std::cout << "AVE\t" << (sum / 10.0) * 100.0 << "\n";
        }

    private:
        static std::string format(std::string root, std::string foldername, std::string filename, std::string number);
        static torch::Tensor skip_fold(const torch::Tensor &src, int fold, int fold_size, bool vec = false);
        static float get_accuracy(const torch::Tensor &scores, const std::vector<int> &flags, const float &threshold);
        static float get_threshold(const torch::Tensor &scores, const std::vector<int> &flags, int thr_num);
        static void get_val_flags(const std::vector<int> &flags, int fold, int fold_size, std::vector<int> &val_flags);
        static void get_test_flags(const std::vector<int> &flags, int fold, int fold_size, std::vector<int> &test_flags);

        static constexpr int NUM_PAIRS = 6000;
        static constexpr int FOLD_SIZE = 600;
        static constexpr float EPSILON = 0.000001;
    };
} // namespace mobile_facenet

#endif
