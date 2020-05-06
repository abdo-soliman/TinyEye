#include <experimental/filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "argparser/argparser.h"
namespace fs = std::experimental::filesystem;

int main(int argc, char **argv)
{
    // ArgumentParser parser;
    // parser.add_option("--models-dir", 's',"../../models", "path to models directory");
    // parser.add_option("--mtcnn-model", 's',"mtcnn", "path to models directory");
    // parser.add_option("--facenet-model", 's',"facenet_lite.tflite", "path to models directory");
    // parser.add_option("--input", 's', "./input.jpg", "path to input image");

    // parser.parse(argc, argv);
    // std::string filename = "";
    // std::string mtcnn_model = "";
    // std::string tf_model = "";
    // std::string model_dir = "";

    // parser.get_option("--models-dir", model_dir);
    // parser.get_option("--mtcnn-model", mtcnn_model);
    // parser.get_option("--facenet-model", tf_model);
    // parser.get_option("--input", filename);

    // std::cout << filename << "\n";

    // cv::Mat frame = cv::imread(filename);
    // cv::imshow("Input Image", frame);
    // cv::waitKey(0);
    // frame.release();
    std::string path = "../data/faces";
    for (const auto &entry : fs::directory_iterator(path))
        std::cout << entry.path() << std::endl;

    return 0;
}
