#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "config.h"
#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/lfw_evaluator.h"
#include "mobilefacenet/dataloader/lfw_loader.h"
using namespace mobile_facenet;

int main()
{
    std::vector<int> flags;
    std::vector<std::string> imgl_list;
    std::vector<std::string> imgr_list;

    std::string lfw_root = "/home/goodmachine/Downloads/TinyEye/Recognition System/data/lfw";
    lfw_evaluator::parse_pairs(lfw_root, "lfw-112x112", "pairs.txt", imgl_list, imgr_list, flags);

    lfw_loader dataset = lfw_loader(imgl_list, imgr_list);
    mobilefacenet network(config.features);
    network->to(config.device);
    lfw_evaluator::evaluate(network, dataset, flags);

    return 0;
}
