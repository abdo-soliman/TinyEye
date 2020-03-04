#ifndef CLASSIFIER_H_H_
#define CLASSIFIER_H_H_

#include <iostream>
#include <string>
#include <map>
#include "utils.h"

class Classifier
{
private:
    static constexpr char DELIMITER[] = "=";

    std::map<uint32_t, std::string> class_map;
    void construct_map(std::string filename);

public:
    Classifier(std::string class_map_path);
};

#endif
