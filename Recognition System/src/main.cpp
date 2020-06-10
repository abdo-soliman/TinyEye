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

    auto svm_model = tinyeye::svm(128, 9);
    svm_model.construct_map("../../models/classes_map.txt");
    svm_model.load("../../models/svm_model.pt");

    MTCNN detector("../../models/mtcnn");

    clock_t start = clock();
    cv::Mat img = cv::imread(argv[1]);
    std::vector<Bbox> boxes = detector.detect(img);

    long prediction;
    std::string name;
    torch::Tensor embeddings;
    
    cv::Mat face;
    cv::Mat img_copy = img.clone();
    for (const auto& box : boxes)
    {
        if (box.score < 0.95)
            continue;

        face = Mat(img, cv::Rect(box.y1, box.x1, box.height, box.width));
        cv::resize(face, face, cv::Size(config.image_width, config.image_height));
        embeddings = net.get_embeddings(face);

        prediction = svm_model.predict(embeddings);
        name = svm_model.prediction_to_class(prediction);

        cv::putText(img_copy, name, cv::Point(box.y1, box.x1), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(255, 0, 0), 1);
    }

    std::cout << "time: " << ((double)clock() - start) / CLOCKS_PER_SEC << std::endl;
    cv::imshow("result", img_copy);
    cv::waitKey(0);

    return 0;
}
