#include "mfn.h"

#include <iostream>

#include "config.h"
#include "dataloaders/img_loader.h"

namespace tinyeye
{
namespace mobilefacenet
{
mfn::mfn(std::string model_path)
{
    try
    {
        torch::load(net, model_path);
        net->eval();
    }
    catch (const c10::Error &e)
    {
        std::cerr << "Can't load mobilefacenet model please make sure the model path is correct!!!" << std::endl;
        exit(-1);
    }
}

torch::Tensor mfn::get_embeddings(torch::Tensor imgs)
{
    return net->forward(imgs);
}

torch::Tensor mfn::get_embeddings(std::vector<cv::Mat>& imgs)
{
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

    return net->forward(imgs_tensor);
}

torch::Tensor mfn::get_embeddings(cv::Mat& img)
{
    auto img_tensor = img_loader::img_to_tensor(img);
    img_tensor = img_tensor.view({1, 3, config.image_height, config.image_width});

    return net->forward(img_tensor);
}
} // namespace mobilefacenet
} // namespace tinyeye
