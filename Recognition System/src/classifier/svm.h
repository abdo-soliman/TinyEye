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
    svm(int in_features=128, int out_features=3, float unknown_th=0.85, int batch_size=32);
    torch::Tensor forward(torch::Tensor x);

    void fit(torch::Tensor dataset, torch::Tensor labels, bool log=false, int iterations=30);
    long predict(torch::Tensor embeddings);
    std::string predict_one(torch::Tensor embeddings);
    std::vector<std::string> predict_many(torch::Tensor embeddings);
    std::string predict_block(torch::Tensor embeddings);

    void construct_map(std::string map_filename);
    std::string prediction_to_class(long prediction);
    
    void load(std::string model_path);
    void save(std::string model_path);
    
    float final_acc;

private:
    static constexpr char DELIMITER[] = "=";
    std::map<long, std::string> class_map;

    int BATCH_SIZE;
    float unknown_threshold;
    int input_features, output_features;

    torch::nn::Linear fully_connected{ nullptr };
};
}

#endif
