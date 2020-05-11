#include "classifier.h"
#include <fstream>
#include "utils/utils.h"

/**
 * @param   none
 * @return  void
 * function to read class names encoding file to constrcut mapping dictionary
 */
void Classifier::construct_map(std::string filename)
{
    std::ifstream file;

    try
    {
        file.open(filename);
        if (!file)
            throw "Error: Can't open class map file please make sure correct path is provided";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    std::string line;
    std::string class_name;
    uint32_t class_code;
    while (file >> line)
    {
        size_t pos = 0;
        // check if delimiter exists
        if ((pos = line.find(DELIMITER)) == std::string::npos)
            throw "Error: Wrong file format can't find delimiter";

        class_name = line.substr(0, pos);
        line.erase(0, pos + 1);
        if (!is_uint(line)) // check if class code is an unsigned integer
            throw "Error: Class code must be an integer";

        class_code = std::stoi(line.c_str());

        class_map.insert(std::pair<uint32_t, std::string>(class_code, class_name));
    }
}

Classifier::Classifier(std::string class_map_path)
{
    construct_map(class_map_path);
}
