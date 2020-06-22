#include <iostream>

#include "mtcnn/mtcnn.h"
#include "classifier/svm.h"
#include "mobilefacenet/config.h"
#include "mobilefacenet/mobilefacenet.h"
#include "mobilefacenet/dataloaders/img_loader.h"
using namespace tinyeye::mtcnn;
using namespace tinyeye::mobilefacenet;

void process(MTCNN& detector, MobileFacenet& net, tinyeye::svm& classifier, cv::Mat& img)
{
    std::vector<Bbox> boxes = detector.detect(img);

    std::string name;
    int x1, y1, width, height;
    cv::Mat face;
    torch::Tensor embeddings;
    for (const auto& box : boxes)
    {
        if (box.score < 0.95)
            continue;

        x1 = std::max(0, box.x1);
        y1 = std::max(0, box.y1);
        width = std::min(img.cols - y1, box.y2 - box.y1 + 1);
        height = std::min(img.rows - x1, box.x2 - box.x1 + 1);
        face = Mat(img, cv::Rect(y1, x1, width, height));

        embeddings = net.get_embeddings(face);
        name = classifier.predict_one(embeddings);

        cv::putText(img, name, cv::Point(y1, x1), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(255, 0, 0), 1);
    }
}

int main(int argc, char **argv)
{
    MTCNN detector("../../models/mtcnn");
    MobileFacenet net("../../models/mobilefacenet.pt");
    auto classifier = tinyeye::svm(128, 5, 0.7);
    classifier.construct_map("../../models/classes_map.txt");
    classifier.load("../../models/svm_model.pt");

    cv::VideoCapture cap;
    std::string ip = argv[1];
    std::string url = "http://" + ip + ":4747/video?x.mjpeg";
    
    cap.open(url);
    if (!cap.isOpened())
    {
        std::cout << "can't read camera feed please make sure the ip is correct" << std::endl;
        return -1;
    }

    cv::namedWindow("ip", cv::WINDOW_AUTOSIZE);

    cv::Mat frame;
    while (true)
    {
        if (!cap.read(frame))
        {
            std::cout << "frame droped!!!" << std::endl;
            continue;
        }

        cv::rotate(frame, frame, cv::ROTATE_90_COUNTERCLOCKWISE);
        process(detector, net, classifier, frame);
        cv::imshow("feed", frame);

        if (cv::waitKey(100) == 27)
            break;
    }

    return 0;
}
