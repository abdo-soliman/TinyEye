#include <memory>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "mtcnn/mtcnn.h"
#include "argparser/argparser.h"

int main (int argc, char** argv)
{
    ArgumentParser parser;
    parser.add_option("--models-dir", 's',"../../models/mtcnn", "path to models directory");
    parser.add_option("--threshold", 'f',"0.99", "MTCNN box score threshold");
    parser.add_option("--map-path", 's',"map.txt", "map file containing all images and number of faces");

    parser.parse(argc, argv);

    std::string models_dir = parser.get_option<std::string>("--models-dir");
    float threshold = parser.get_option<float>("--threshold");
    std::string filename = parser.get_option<std::string>("--map-path");

    std::unique_ptr<mtcnn::MTCNN> detector;
    try
    {
        detector.reset();
        detector = std::make_unique<mtcnn::MTCNN>(models_dir);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::ifstream input_file;
    double num_precision = 0;
    double num_recall = 0;
    double total = 0;
    try
    {
        input_file.open(filename);
        if (input_file.is_open())
        {
            cv::Mat img;
            int num_faces;
            int predicted_num_faces;
            std::string image_path;
            while (input_file >> image_path >> num_faces)
            {
                total++;
                img = cv::imread(image_path);
                predicted_num_faces = detector->detect_count(img, threshold);
                if (predicted_num_faces == num_faces)
                    num_precision++;
                if (predicted_num_faces >= num_faces)
                    num_recall++;
            }

            std::cout << "Total Number Of Images: " << total << std::endl;
            std::cout << "Number Of Precision classified Images: " << num_precision << std::endl;
            std::cout << "Number Of Recall classified Images: " << num_recall << std::endl;
            std::cout.precision(4);
            std::cout << "Precision: " << (num_precision / total) * 100.0 << "%" << std::endl;
            std::cout << "Recall: " << (num_recall / total) * 100.0 << "%" << std::endl;
        }
        else
        {
            std::cerr << "Error: can't open file please make sure input_file path is correct\n";
            exit(-2);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        exit(-3);
    }

    return 0;
}
