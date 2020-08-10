#include "recognition_system.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "mobilefacenet/config.h"

namespace tinyeye
{
    RecognitionSystem::RecognitionSystem(std::string mtcnn_md, std::string mfn_model_path, bool detect, int batch_size)
    {
        try
        {
            mfn_net.reset();
            mfn_net = std::make_unique<mobilefacenet::mfn>(mfn_model_path);

            use_mtcnn = detect;
            detector_config.models_dir = mtcnn_md;

            BATCH_SIZE = batch_size;
            classifier_config.batch_size = batch_size;
        }
        catch (const std::exception& e)
        {
            logger::LOG_ERROR(FAILED_TO_INTIALIZE_MOBILE_FACENET, e.what());
            add_error(FAILED_TO_INTIALIZE_MOBILE_FACENET, "failed to load mobilefacenet network with an exception see ~/tinyeye-server.log for more details");
            construction_failed = true;
        }
    }

    void RecognitionSystem::add_error(ErrorCode code, std::string msg)
    {
        errors.push_back(std::make_pair(code, msg));
    }

    bool RecognitionSystem::train(std::string train_map_path, std::string test_map_path, std::string delimiter, int num_t, bool log, int iters)
    {
        std::pair<torch::Tensor, torch::Tensor> trainset, testset;
        try
        {
            mobilefacenet::img_loader train_dataset(train_map_path, delimiter);
            if (use_mtcnn)
            {
                auto train_warnings = train_dataset.use_mtcnn(detector_config);

                for (const auto& warning : train_warnings)
                {
                    if (warning.first == CLASS_DROPED)
                    {
                        logger::LOG_ERROR(FAILED_TO_CREATE_TRAINSET, warning.second);
                        add_error(FAILED_TO_CREATE_TRAINSET, warning.second);
                        return false;
                    }
                    else
                    {
                        warnings.push_back(warning);
                        logger::LOG_WARNING(warning.first, warning.second);
                    }
                }
            }

            num_classes = train_dataset.get_num_classes();
            classifier_config.out_features = num_classes;
            trainset_size = train_dataset.size().value();
            trainset = create_dataset(train_dataset);
        }
        catch (const std::exception& e)
        {
            logger::LOG_ERROR(FAILED_TO_CREATE_TRAINSET, e.what());
            add_error(FAILED_TO_CREATE_TRAINSET, "failed to create training embeddings with an exception see ~/tinyeye-server.log for more details");
            return false;
        }
        auto train_embeddings = trainset.first;
        auto train_labels = trainset.second;

        try
        {
            mobilefacenet::img_loader test_dataset(test_map_path, delimiter);
            if (use_mtcnn)
            {
                auto test_warnings = test_dataset.use_mtcnn(detector_config);

                for (const auto& warning : test_warnings)
                {
                    warnings.push_back(warning);
                    logger::LOG_WARNING(warning.first, warning.second);
                }
            }
            testset_size = test_dataset.size().value();
            testset = create_dataset(test_dataset);
        }
        catch (const std::exception& e)
        {
            logger::LOG_ERROR(FAILED_TO_CREATE_TESTSET, e.what());
            add_error(FAILED_TO_CREATE_TESTSET, "failed to create test embeddings with an exception see ~/tinyeye-server.log for more details");
            return false;
        }
        auto test_embeddings = testset.first;
        auto test_labels = testset.second;

        int test_size = test_labels.sizes()[0];
        float test_acc, best_acc = 0.0f;
        float min_loss = 0.0f;

        avg_trial_time = 0;
        int i, best_index;
        for (i = 0; i < num_t; i++)
        {
            if (log)
                logger::LOG_INFO("starting training trial number " + std::to_string(i) + "...");
            ArcFace current_classifier(classifier_config);

            clock_t start = clock();
            current_classifier.fit(train_embeddings, train_labels, log, iters);
            float svm_train_time = static_cast<float>(clock() - start) / CLOCKS_PER_SEC;
            avg_trial_time += svm_train_time;

            auto predicted_labels = current_classifier.predict_many(test_embeddings);
            test_acc = predicted_labels.eq(test_labels).to(torch::kLong).sum().item<float>();
            test_acc /= test_size;

            if (log)
                logger::LOG_INFO("finshed training trial number " + std::to_string(i) + " with training_loss: " + std::to_string(current_classifier.get_loss()));

            std::string msg = "Trial: " + std::to_string(i) +\
                ", Acc: " + std::to_string(test_acc) +\
                ", Time: " + std::to_string(svm_train_time);
            logger::LOG_INFO(msg);

            if (test_acc > best_acc || current_classifier.get_loss() < min_loss)
            {
                best_index = i;
                best_acc = test_acc;
                min_loss = current_classifier.get_loss();
                classifier.reset();
                classifier = std::make_unique<ArcFace>(current_classifier);

                if (test_acc > 0.985 && min_loss < 0.003)
                    break;
            }
        }

        num_trials = num_t;
        num_excuted_trials = i + 1;
        best_trial = best_index;

        iterations = iters;
        avg_trial_time /= i + 1;

        model_acc = best_acc;

        return true;
    }

    std::pair<torch::Tensor, torch::Tensor> RecognitionSystem::create_dataset(mobilefacenet::img_loader& dataset)
    {
        long size = dataset.size().value();
        auto loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
            std::move(dataset.map(torch::data::transforms::Stack<>())), BATCH_SIZE);

        torch::Tensor labels;
        torch::Tensor embeddings;
        int index = 0;
        int current_batch_size = 0;
        for (auto& batch : *loader)
        {
            auto data = batch.data.to(mobilefacenet::config.device);
            auto targets = batch.target.to(mobilefacenet::config.device).view({ -1 });
            current_batch_size = targets.sizes()[0];

            auto current_embeddings = mfn_net->get_embeddings(data);

            if (index == 0)
            {
                embeddings = current_embeddings;
                labels = targets;
            }
            else if (BATCH_SIZE*(index+1) >= size)
            {
                embeddings = torch::cat({ embeddings, current_embeddings }).view({ size, 128 }).to(torch::kFloat);
                labels = torch::cat({ labels, targets }).view({ size }).to(torch::kLong);
            }
            else
            {
                embeddings = torch::cat({ embeddings, current_embeddings }).view({ (index+1)*current_batch_size, 128 }).to(torch::kFloat);
                labels = torch::cat({ labels, targets }).view({ (index+1)*current_batch_size }).to(torch::kLong);
            }

            index++;
        }

        return std::make_pair(embeddings, labels);
    }

    bool RecognitionSystem::save_classifier(std::string path)
    {
        if (!classifier)
        {
            logger::LOG_ERROR(INVALID_CLASSIFIER, "classifier doesn't exist");
            add_error(INVALID_CLASSIFIER, "classifier doesn't exist");
            return false;
        }

        if (!classifier->is_trained())
        {
            logger::LOG_ERROR(INVALID_CLASSIFIER, "classifier is not trained");
            add_error(INVALID_CLASSIFIER, "classifier is not trained");
            return false;
        }

        try
        {
            classifier->save(path);
            model_path = path;
            return true;
        }
        catch (const std::exception& e)
        {
            logger::LOG_ERROR(FAIILED_TO_SAVE_CLASSIFIER, e.what());
            add_error(FAIILED_TO_SAVE_CLASSIFIER, "failed to write model at " + path + " is either ont path or not writable");
            return false;
        }

        return false;
    }

    void RecognitionSystem::create_log(std::string log_file_path)
    {
        std::ofstream log_file;
        log_file.open(log_file_path.c_str());
        if (log_file.is_open())
        {
            log_file << "{\n";

            // adding errors
            log_file << "\t\"errors\": ";
            if (errors.empty())
                log_file << "null,\n";
            else
            {
                log_file << "[\n";
                for (size_t i = 0; i < errors.size(); i++)
                {
                    if (i == errors.size() - 1)
                        log_file << "\t\t{ \"code\": \"" + logger::error2str(errors[i].first) + "\", \"message\": \"" + errors[i].second + "\" }\n";
                    else
                        log_file << "\t\t{ \"code\": \"" + logger::error2str(errors[i].first) + "\", \"message\": \"" + errors[i].second + "\" },\n";
                }
                log_file << "\t],\n";
            }

            // adding warnings
            log_file << "\t\"warnings\": ";
            if (warnings.empty())
                log_file << "null,\n";
            else
            {
                log_file << "[\n";
                for (size_t i = 0; i < warnings.size(); i++)
                {
                    if (i == warnings.size() - 1)
                        log_file << "\t\t{ \"code\": \"" + logger::warning2str(warnings[i].first) + "\", \"message\": \"" + warnings[i].second + "\" }\n";
                    else
                        log_file << "\t\t{ \"code\": \"" + logger::warning2str(warnings[i].first) + "\", \"message\": \"" + warnings[i].second + "\" },\n";
                }
                log_file << "\t],\n";
            }

            // adding Unix timestamp
            std::time_t result = std::time(nullptr);
            log_file << "\t\"timestamp\": " << result << ",\n";

            // adding use_mtcnn tag value
            log_file << "\t\"useMTCNN\": ";
            if (use_mtcnn)
                log_file << "true,\n";
            else
                log_file << "false,\n";

            // adding datasets info
            log_file << "\t\"numClasses\": " << num_classes << ",\n";
            log_file << "\t\"trainSetSize\": " << trainset_size << ",\n";
            log_file << "\t\"testSetSize\": " << testset_size << ",\n";

            // adding training settings
            log_file << "\t\"numTrials\": " << num_trials << ",\n";
            log_file << "\t\"numExcutedTrials\": " << num_excuted_trials << ",\n";
            log_file << "\t\"bestTrial\": " << best_trial << ",\n";

            // adding trial settings
            log_file << "\t\"numIterationsPerTrial\": " << iterations << ",\n";
            log_file << "\t\"batchSize\": " << BATCH_SIZE << ",\n";
            log_file << "\t\"avgTrialTime\": " << avg_trial_time << ",\n";

            // adding result model info
            log_file << "\t\"modelAcc\": " << model_acc << ",\n";
            log_file << "\t\"modelPath\": \"" << model_path << "\"\n";

            log_file << "}\n";
        }
        else
            throw std::runtime_error("can't create result log file");
    }
} // namespace tinyeye
