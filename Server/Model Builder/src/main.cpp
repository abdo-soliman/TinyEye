#include <iostream>

#include "argparser/argparser.h"
#include "recognition_system/recognition_system.h"

int main(int argc, char** argv)
{
    tinyeye::ArgumentParser parser;
    parser.add_option("--server-log-path", 's', "~/tinyeye-server.log", "path to write server log");
    parser.add_option("--mtcnn-models-dir", 's', "../models/mtcnn", "path to mtcnn models directory, ignore if --use-mtcnn is set to false");
    parser.add_option("--mfn-model-path", 's', "../models/mobilefacenet.pt", "path to MobileFaceNet model");
    parser.add_option("--train-map-path", 's', "~/data/train/map.txt", "path to trainset mapping file");
    parser.add_option("--test-map-path", 's', "~/data/test/map.txt", "path to testset mapping file");
    parser.add_option("--mapping-file-delimiter", 's', " ", "delimiter between image path and label in both train and test mapping files");
    parser.add_option("--output-model-path", 's', "./svm_model.pt", "path to testset mapping file");
    parser.add_option("--batch-size", 'i', "32", "training batch size");
    parser.add_option("--use-mtcnn", 'b', "true", "set true to perform MTCNN face detection on input images");
    parser.add_option("--num-trials", 'i', "20", "number of models to train and choose the one with highest test accuracy");
    parser.add_option("--log", 'b', "false", "log every training epoch loss, accuracy and time for all trials");
    parser.add_option("--num-iterations", 'i', "30", "number of training iterations per trial");
    parser.add_option("--json-log-path", 's', "./result.json", "path to json result log file");
    parser.add_option("--json-codes-path", 's', "", "path to json codes file set with file path to get error codes and warning codes in json format");

    parser.parse(argc, argv);

    tinyeye::logger::set_logfile_path(parser.get_option<std::string>("--server-log-path"));

    std::string codes_path = parser.get_option<std::string>("--json-codes-path");
    if (codes_path != "")
    {
        tinyeye::logger::json_codes(codes_path);
        return 0;
    }

    std::string mtcnn_models_dir = parser.get_option<std::string>("--mtcnn-models-dir");
    std::string mobilefacenet_model_path = parser.get_option<std::string>("--mfn-model-path");
    std::string train_map_path = parser.get_option<std::string>("--train-map-path");
    std::string test_map_path = parser.get_option<std::string>("--test-map-path");
    std::string delimiter = parser.get_option<std::string>("--mapping-file-delimiter");
    std::string output_model_path = parser.get_option<std::string>("--output-model-path");
    std::string json_log_path = parser.get_option<std::string>("--json-log-path");

    int batch_size = parser.get_option<int>("--batch-size");
    bool use_mtcnn = parser.get_option<bool>("--use-mtcnn");

    int num_trials = parser.get_option<int>("--num-trials");
    bool log = parser.get_option<bool>("--log");
    int iterations = parser.get_option<int>("--num-iterations");

    tinyeye::RecognitionSystem recognition_system(mtcnn_models_dir, mobilefacenet_model_path, use_mtcnn, batch_size);
    if (!recognition_system.construction_failed)
    {
        try
        {
            bool successful = recognition_system.train(train_map_path, test_map_path, delimiter, num_trials, log, iterations);
            if (successful)
                recognition_system.save_classifier(output_model_path);
        }
        catch(const std::exception& e)
        {
            tinyeye::logger::LOG_ERROR(tinyeye::UNKNOWN, e.what());
            recognition_system.add_error(tinyeye::UNKNOWN, "failed to train and save the model with an exception see ~/tinyeye-server.log for more details");
        }
    }

    recognition_system.create_log(json_log_path);

    return 0;
}
