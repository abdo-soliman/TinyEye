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
    static std::vector<float> evaluation_10_fold(const torch::Tensor& left_features, const torch::Tensor& right_features,
                                                 const std::vector<int>& flags);
    static void evaluate(mobilefacenet& network, lfw_loader& dataset, const std::vector<int>& flags);

private:
    static std::string format(std::string root, std::string foldername, std::string filename, std::string number);
    static torch::Tensor skip_fold(const torch::Tensor &src, int fold, int fold_size, bool vec = false);
    static float get_accuracy(const torch::Tensor &scores, const std::vector<int> &flags, const float &threshold);
    static float get_threshold(const torch::Tensor &scores, const std::vector<int> &flags, int thr_num);
    static void get_val_flags(const std::vector<int>& flags, int fold, int fold_size, std::vector<int>& val_flags);
    static void get_test_flags(const std::vector<int>& flags, int fold, int fold_size, std::vector<int>& test_flags);

    static constexpr int NUM_PAIRS = 6000;
    static constexpr int FOLD_SIZE = 600;
    static constexpr float EPSILON = 0.000001;
};
} // namespace mobile_facenet

#endif
