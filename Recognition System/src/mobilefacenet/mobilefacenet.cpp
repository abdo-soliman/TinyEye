#include "mobilefacenet.h"

#include <iostream>

#include "config.h"
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

torch::Tensor MobileFacenet::get_embeddings(std::vector<cv::Mat> &imgs)
{
    std::vector<torch::jit::IValue> inputs;
    torch::Tensor imgs_tensor, temp;
    int index = 1;
    for (auto &img : imgs)
    {
        temp = img_loader::img_to_tensor(img).view({1, 3, config.image_height, config.image_width});
        if (index == 1)
            imgs_tensor = temp;
        else
        {
            imgs_tensor = torch::cat({imgs_tensor, temp})
                                .view({index, 3, config.image_height, config.image_width})
                                .to(torch::kFloat);
        }
        index++;
    }
    inputs.push_back(imgs_tensor);

    return net.forward(inputs).toTensor();
}

torch::Tensor MobileFacenet::get_embeddings(cv::Mat &img)
{
    std::vector<torch::jit::IValue> inputs;
    auto img_tensor = img_loader::img_to_tensor(img);
    img_tensor = img_tensor.view({1, 3, config.image_height, config.image_width});
    inputs.push_back(img_tensor);

    return net.forward(inputs).toTensor();
}
} // namespace mobilefacenet
} // namespace tinyeye
