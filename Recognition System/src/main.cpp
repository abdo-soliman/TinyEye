#include <iostream>

#include "classifier/svm.h"
#include "mobilefacenet/config.h"
#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/dataloaders/img_loader.h"
using namespace tinyeye::mobilefacenet;

std::pair<torch::Tensor, torch::Tensor> create_dataset(std::string map_filepath, MobileFacenet& net, int batch_size)
{
    auto dataset = img_loader(map_filepath);
    long size = dataset.size().value();

    auto loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
        std::move(dataset.map(torch::data::transforms::Stack<>())), batch_size);

    torch::Tensor labels;
    torch::Tensor embeddings;
    int index = 0;
    int current_batch_size = 0;
    for (auto& batch : *loader)
    {
        auto data = batch.data.to(config.device);
        auto targets = batch.target.to(config.device).view({-1});
        current_batch_size = targets.sizes()[0];

        auto current_embeddings = net.get_embeddings(data);

        if (index == 0)
        {
            embeddings = current_embeddings;
            labels = targets;
        }
        else if (batch_size*(index+1) >= size)
        {
            embeddings = torch::cat({embeddings, current_embeddings}).view({size, 128}).to(torch::kFloat);
            labels = torch::cat({labels, targets}).view({size}).to(torch::kLong);
        }
        else
        {
            embeddings = torch::cat({embeddings, current_embeddings}).view({(index+1)*current_batch_size, 128}).to(torch::kFloat);
            labels = torch::cat({labels, targets}).view({(index+1)*current_batch_size}).to(torch::kLong);
        }

        index++;
    }

    return std::make_pair(embeddings, labels);
}

int main(int argc, char **argv)
{
    MobileFacenet net("../../models/mobilefacenet.pt");
    auto train_data = create_dataset("../../data/train_map.txt", net, config.train_batch_size);
    auto train_embeddings = train_data.first;
    auto train_labels = train_data.second;

    auto svm_model = tinyeye::svm("../../models/classes_map.txt", 128, 9);
    svm_model.fit(train_embeddings, train_labels);

    auto test_data = create_dataset("../../data/test_map.txt", net, config.test_batch_size);
    auto test_embeddings = test_data.first;
    auto test_labels = test_data.second;

    float correct = 0;
    int size = test_labels.sizes()[0];
    for (int i = 0; i < size; i++)
    {
        auto prediction = svm_model.predict(test_embeddings.narrow(0, i, 1));
        auto real = (test_labels.narrow(0, i, 1)).item<long>();

        if (prediction == real)
            correct++;
    }

    std::cout << "training acc: " << svm_model.final_acc*100 << "%" << std::endl;
    std::cout << "test acc: " << (correct / size) * 100 << "%" << std::endl;

    return 0;
}
