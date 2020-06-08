#ifndef SVM_H_H_
#define SVM_H_H_

#include <map>
#include <string>
#include <torch/torch.h>

namespace tinyeye
{
class svm : torch::nn::Module
{
public:
    svm(std::string map_filename, int in_features=128, int out_features=3, int batch_size=32);
    torch::Tensor forward(torch::Tensor x);
    void fit(torch::Tensor dataset, torch::Tensor labels, bool log=false, int iterations=30);
    long predict(torch::Tensor embeddings);
    std::string prediction_to_class(long prediction);
    float final_acc;

private:
    void construct_map(std::string map_filename);

    static constexpr char DELIMITER[] = "=";
    int BATCH_SIZE;
    int output_features;
    std::map<long, std::string> class_map;
    torch::nn::Linear fully_connected{ nullptr };
};

}

#endif
