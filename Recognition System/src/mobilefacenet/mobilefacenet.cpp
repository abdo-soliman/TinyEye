#include "mobilefacenet.h"

#include <iostream>

#include "dataloaders/img_loader.h"

namespace tinyeye
{
namespace mobilefacenet
{
MobileFacenet::MobileFacenet(std::string model_path)
{
    try
    {
        net = torch::jit::load(model_path);
        net.eval();
    }
    catch (const c10::Error &e)
    {
        std::cerr << "Can't load mobilefacenet model please make sure the model path is correct!!!" << std::endl;
        exit(-1);
    }
}

torch::Tensor MobileFacenet::get_embeddings(torch::Tensor imgs)
{
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(imgs);

    return net.forward(inputs).toTensor();
}

// torch::Tensor MobileFacenet::get_embeddings(std::vector<cv::Mat>& imgs)
// {
//     std::vector<torch::jit::IValue> inputs;
//     for (auto& img : imgs)
//         inputs.push_back(img_loader::img_to_tensor(img));

//     return net.forward(inputs).toTensor();
// }

// torch::Tensor MobileFacenet::get_embeddings(cv::Mat img)
// {
//     std::vector<torch::jit::IValue> inputs;
//     inputs.push_back(img_loader::img_to_tensor(img));

//     return net.forward(inputs).toTensor();
// }
} // mobilefacenet
} // tinyeye
