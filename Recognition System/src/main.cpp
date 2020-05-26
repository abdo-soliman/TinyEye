#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "config.h"
#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/lfw_evaluator.h"
using namespace mobile_facenet;

int main()
{
    mobilefacenet network(config.features);
    network->to(config.device);

    std::string lfw_root = "/home/goodmachine/Downloads/TinyEye/Recognition System/data/lfw";
    lfw_evaluator::evaluate(network, lfw_root, "lfw-96x112", "pairs.txt", "whatever");
    return 0;
}
