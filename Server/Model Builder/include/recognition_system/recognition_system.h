#ifndef RECOGNITION_SYSTEM_H_H_
#define RECOGNITION_SYSTEM_H_H_

#include <string>
#include <vector>
#include <torch/torch.h>
#include <opencv2/opencv.hpp>

#include "logger/logger.h"
#include "mtcnn/mtcnn.h"
#include "classifier/svm.h"
#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/dataloaders/img_loader.h"

namespace tinyeye
{
class RecognitionSystem
{
public:
    RecognitionSystem(std::string mtcnn_md, std::string mfn_model_path, bool detect=true, int batch_size=32);
    void add_error(ErrorCode code, std::string msg);

    bool train(std::string train_map_path, std::string test_map_path, int num_trials=100, bool log=false, int interations=30);
    bool save_classifier(std::string path);
    void create_log(std::string log_file_path);

    bool construction_failed = false;

private:
    std::pair<torch::Tensor, torch::Tensor> create_dataset(mobilefacenet::img_loader& dataset);

    std::unique_ptr<mobilefacenet::MobileFacenet> mfn_net;
    std::unique_ptr<svm> classifier;

    svm_config classifier_config;

    bool use_mtcnn = true;
    mtcnn::mtcnn_config detector_config;

    int BATCH_SIZE = 32;

    std::vector<std::pair<ErrorCode, std::string>> errors;
    std::vector<std::pair<WarningCode, std::string>> warnings;
    long num_classes = 0;
    long trainset_size = 0;
    long testset_size = 0;
    int num_trials = 0;
    int iterations = 0;
    int num_excuted_trials = 0;
    int best_trial = 0;
    float avg_trial_time = 0.0f;
    float model_acc = 0.0f;
    std::string model_path = "./svm_model.pt";
};
} // tinyeye

#endif
