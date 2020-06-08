#include <iostream>

#include "mtcnn/mtcnn.h"
#include "classifier/svm.h"
#include "mobilefacenet/config.h"
#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/dataloaders/img_loader.h"
using namespace tinyeye::mtcnn;
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
    svm_model.fit(train_embeddings, train_labels, true, 5);

    MTCNN detector("../../models/mtcnn");
    cv::Mat img = cv::imread(argv[1]);
    std::vector<cv::Mat> faces = detector.detect_faces(img, 0.95);

    std::string name;
    cv::Mat scaled_face;
    for (const auto& face : faces)
    {
        cv::resize(face, scaled_face, cv::Size(config.image_width, config.image_height));
        auto test_embeddings = net.get_embeddings(scaled_face);

        auto prediction = svm_model.predict(test_embeddings);
        name = svm_model.prediction_to_class(prediction);

        cv::imshow(name, scaled_face);
        cv::waitKey(0);
    }

    return 0;
}
