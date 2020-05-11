#include <iostream>
#include <opencv2/opencv.hpp>
#include "argparser/argparser.h"
#include "mtcnn/mtcnn.h"

int main (int argc, char** argv)
{
    ArgumentParser parser;
    parser.add_option("--models-dir", 's',"../../models", "path to models directory");
    parser.add_option("--mtcnn-model", 's',"mtcnn", "path to models directory");
    parser.add_option("--facenet-model", 's',"facenet_lite.tflite", "path to models directory");
    parser.add_option("--input", 's', "./input.jpg", "path to input image");
    
    parser.parse(argc, argv);
	std::string filename = "";
    std::string mtcnn_model = "";
    std::string tf_model = "";
    std::string models_dir = "";

    parser.get_option("--models-dir", models_dir);
    parser.get_option("--mtcnn-model", mtcnn_model);
    parser.get_option("--facenet-model", tf_model);
    parser.get_option("--input", filename);

    mtcnn_model = models_dir + '/' + mtcnn_model;

    try
    {
        cv::Mat img = cv::imread(filename);
        mtcnn::MTCNN detector(mtcnn_model);
        detector.detect_draw(img);
        cv::imshow("result", img);
        cv::waitKey(0);
        img.release();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
