#include "utils.h"
#include "ArcFace.h"
#include <torch/torch.h>

namespace tinyeye
{
ArcFace::ArcFace(float unknown_th)
{
    unknown_threshold = unknown_th;
}

ArcFace::ArcFace(ArcMarginProductConfig config, float unknown_th)
{
    classifier = ArcMarginProduct(config.in_features, config.out_features, config.s, config.m, config.easy_margin);
    unknown_threshold = unknown_th;
    BATCH_SIZE = config.batch_size;
    num_classes = config.out_features;
}

void ArcFace::fit(torch::Tensor dataset, torch::Tensor labels, bool log, int iterations)
{
    classifier->train();
    auto optimizer = new torch::optim::SGD(
        classifier->parameters(),
        torch::optim::SGDOptions(0.1).momentum(0.9).weight_decay(0.0004).nesterov(true)
    );

    int N = labels.sizes()[0];
    if (BATCH_SIZE > N)
        BATCH_SIZE = N;

    int num_loops = ceil((float)N / BATCH_SIZE);

    int start;
    float sum_loss;
    torch::Tensor embeddings, label;
    auto criterion = torch::nn::CrossEntropyLoss();
    for (int epoch = 0; epoch < iterations; epoch++)
    {
        sum_loss = 0.0f;
        for (int i = 0; i < num_loops; i++)
        {
            start = i * BATCH_SIZE;
            if (i == num_loops - 1)
            {
                embeddings = dataset.narrow(0, start, N-start);
                label = labels.narrow(0, start, N-start);
            }
            else
            {
                embeddings = dataset.narrow(0, start, BATCH_SIZE);
                label = labels.narrow(0, start, BATCH_SIZE);
            }

            optimizer->zero_grad();
            auto output = classifier->forward(embeddings, label);
            auto loss = criterion(output, label);

            loss.backward({}, true);
            optimizer->step();

            sum_loss += loss.template item<float>() * label.sizes()[0];
        }

        if (log)
            std::cout << "Epoch: " << epoch << "\tloss: " << sum_loss / N << std::endl;

        if (epoch == iterations-1)
            training_loss = sum_loss / N;
    }

    trained = true;
}

float ArcFace::get_loss()
{
    return training_loss;
}

bool ArcFace::is_trained()
{
    return trained;
}

long ArcFace::predict(torch::Tensor embeddings)
{
    classifier->eval();
    auto criterion = torch::nn::CrossEntropyLoss();
    float min_loss, loss_value;
    long prediction = 0;

    for (long i = 0; i < num_classes; i++)
    {
        auto label = torch::zeros({1}, torch::kLong);
        label += i;
        auto output = classifier->forward(embeddings, label);
        auto loss = criterion(output, label);

        loss_value = loss.template item<float>();

        if (i == 0)
            min_loss = loss_value;

        if (loss_value < min_loss)
        {
            min_loss = loss_value;
            prediction = i;
        }
    }

    return prediction;
}

torch::Tensor ArcFace::predict_many(torch::Tensor embeddings)
{
    int N = embeddings.sizes()[0];
    std::vector<long> predictions;
    for (int i = 0; i < N; i++)
    {
        auto embedding = embeddings.narrow(0, i, 1);
        predictions.push_back(predict(embedding));
    }

    return torch::from_blob(predictions.data(), {N}, torch::kLong).clone();
}

std::string ArcFace::prediction_to_class(long prediction)
{
    if (prediction == -1)
        return "unknown";

    return class_map[prediction];
}

void ArcFace::construct_map(std::string map_filename)
{
    std::ifstream file;

    try
    {
        file.open(map_filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Can't open class map file please make sure correct path is provided" << std::endl;
            exit(-1);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    std::string line;
    std::string class_name;
    long class_code;
    while (file >> line)
    {
        size_t pos = 0;
        // check if delimiter exists
        if ((pos = line.find(DELIMITER)) == std::string::npos)
            throw "Error: Wrong file format can't find delimiter";

        class_name = line.substr(0, pos);
        line.erase(0, pos + 1);
        if (!utils::is_uint(line)) // check if class code is an unsigned integer
            throw "Error: Class code must be an integer";

        class_code = std::stoi(line.c_str());

        class_map.insert(std::pair<long, std::string>(class_code, class_name));
    }

    num_classes = class_map.size();
}

void ArcFace::load(std::string model_path)
{
    try
    {
        torch::load(classifier, model_path);
        classifier->eval();
    }
    catch (const c10::Error &e)
    {
        std::cerr << "Can't load mobilefacenet model please make sure the model path is correct!!!" << std::endl;
        exit(-1);
    }
}

void ArcFace::save(std::string model_path)
{
    torch::save(classifier, torch::str(model_path));
}
} // namespace tinyeye
