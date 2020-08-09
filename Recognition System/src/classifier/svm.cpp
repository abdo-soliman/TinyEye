#include "svm.h"

#include <cmath>
#include <fstream>
#include <iostream>

#include "utils.h"

namespace tinyeye
{
svm::svm(int in_features, int out_features, float unknown_th, int batch_size)
{
    BATCH_SIZE = batch_size;
    input_features = in_features;
    output_features = out_features;
    unknown_threshold = unknown_th;
}

torch::Tensor svm::forward(torch::Tensor x)
{
    return fully_connected(x);
}

void svm::fit(torch::Tensor dataset, torch::Tensor labels, bool log, int iterations)
{
    fully_connected = register_module("fully_connected", torch::nn::Linear(input_features, output_features));
    this->train();
    auto optimizer = new torch::optim::SGD(this->parameters(), torch::optim::SGDOptions(0.1));

    int N = labels.sizes()[0];
    auto one_hot = torch::zeros({N, output_features});
    one_hot.scatter_(1, labels.view({-1, 1}), 1);

    if (BATCH_SIZE > N)
        BATCH_SIZE = N;

    int num_loops = ceil((float)N / BATCH_SIZE);

    int start;
    float sum_loss, acc;
    torch::Tensor embeddings, label, label_number;
    for (int epoch = 0; epoch < iterations; epoch++)
    {
        sum_loss = 0.0f;
        acc = 0.0f;
        for (int i = 0; i < num_loops; i++)
        {
            start = i * BATCH_SIZE;
            if (i == num_loops - 1)
            {
                embeddings = dataset.narrow(0, start, N-start);
                label = one_hot.narrow(0, start, N-start);
                label_number = labels.narrow(0, start, N-start);
            }
            else
            {
                embeddings = dataset.narrow(0, start, BATCH_SIZE);
                label = one_hot.narrow(0, start, BATCH_SIZE);
                label_number = labels.narrow(0, start, BATCH_SIZE);
            }

            optimizer->zero_grad();
            auto output = forward(embeddings);
            auto loss = torch::mean(torch::clamp(1 - label * output, 0));

            loss.backward({}, true);
            optimizer->step();

            sum_loss += loss.template item<float>();
            acc += output.argmax(1).eq(label_number).to(torch::kLong).sum().item<float>();
        }

        if (log)
            std::cout << "Epoch: " << epoch << "\tloss: " << sum_loss / N << "\tacc: " << acc / N << std::endl;
    }
    final_acc = acc / N;
}

long svm::predict(torch::Tensor embeddings)
{
    this->eval();
    auto output = forward(embeddings);

    if(output.max().item<float>() < unknown_threshold)
        return -1;

    return output.argmax().to(torch::kLong).item<long>();
}

std::string svm::prediction_to_class(long prediction)
{
    if (prediction == -1)
        return "unknown";

    return class_map[prediction];
}

void svm::construct_map(std::string map_filename)
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
}

void svm::load(std::string model_path)
{
    torch::nn::Linear fc = torch::nn::Linear(input_features, output_features);
    torch::load(fc, model_path);
    fully_connected = register_module("fully_connected", fc);
}

void svm::save(std::string model_path)
{
    torch::save(fully_connected, torch::str(model_path));
}
} // namespace tinyeye
