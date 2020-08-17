#ifndef ARC_FACE_H_H_
#define ARC_FACE_H_H_

#include <map>
#include <string>
#include "ArcMarginProduct.h"

namespace tinyeye
{
class ArcFace
{
public:
    ArcFace(float unknown_th=2);
    ArcFace(ArcMarginProductConfig config, float unknown_th=2);

    void fit(torch::Tensor dataset, torch::Tensor labels, bool log=false, int iterations=100);
    float get_loss();
    bool is_trained();

    long predict(torch::Tensor embeddings);
    std::string predict_block(torch::Tensor embeddings);
    torch::Tensor predict_many(torch::Tensor embeddings);

    void construct_map(std::string map_filename);
    std::string prediction_to_class(long prediction);
    
    void load(std::string model_path);
    void save(std::string model_path);
private:
    ArcMarginProduct classifier;
    float training_loss;
    bool trained = false;

    static constexpr char DELIMITER[] = "=";
    std::map<long, std::string> class_map;
    long num_classes;

    int BATCH_SIZE = 32;
    float unknown_threshold;
};
} // namespace tinyeye

#endif
