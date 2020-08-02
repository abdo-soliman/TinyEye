#ifndef MTCNN_H_H_
#define MTCNN_H_H_

// remove or comment the following line if you are using CUDA
#define CPU_ONLY 1

#include <caffe/caffe.hpp>
#include <opencv2/opencv.hpp>
using cv::Mat;
using std::vector;

namespace tinyeye
{
namespace mtcnn
{
template <typename T>
static std::list<size_t> argsort(vector<T> data)
{
    size_t n = 0;
    std::list<size_t> indecies(data.size(), 0);
    std::generate(indecies.begin(), indecies.end(), [&] { return n++; });
    indecies.sort([&data](size_t i1, size_t i2) { return data[i1] < data[i2]; });

    return indecies;
}

struct Bbox
{
    int x1;
    int y1;

    int x2;
    int y2;

    int width;
    int height;

    double score;

    cv::Vec4f reg;
};

class MTCNN
{
public:
    static Mat scale_image(Mat image, double scale);
    MTCNN(const std::string &models_dir, float thresholds[2] = NULL, uint32_t min_fs = 50, double sf = 0.709);
    vector<Bbox> detect(Mat img);
    vector<Mat> detect_faces(Mat image, float threshold=-1);
    void detect_draw(Mat &img);

private:
    // common functions
    void non_maximum_suppression(vector<Bbox> boxes, double threshold, char method, vector<Bbox> &result);
    void box_regress(vector<Bbox> &boxes, int stage);
    void box_to_square(const vector<Bbox> &box);
    void padding(const vector<Bbox> &boxes, int img_w, int img_h);

    // stage 1 functions
    void wrap_input_layer(vector<Mat> *input_channels, caffe::Blob<float> *input_layer, const int height, const int width);
    void compute_scale_pyramid(double m, int min_layer, vector<double> &scales);
    void generate_bbox(caffe::Blob<float> *confidence, caffe::Blob<float> *reg,
                        double scale, float threshold, int image_width, int image_height, vector<Bbox> &boxes);
    void stage1(Mat img, const vector<double> &scales);

    // stage 2 functions
    caffe::Datum mat_to_datum(const Mat &cv_mat);
    vector<Bbox> stage2(Mat img);

    // private data
    boost::shared_ptr<caffe::Net<float>> p_net;
    boost::shared_ptr<caffe::Net<float>> r_net;

    vector<Bbox> regrec_1_result;
    vector<Bbox> padding_1_result;

    uint32_t min_face_size;
    double scale_factor;
    float steps_threshold[2];
    int num_channels;
};
} // namespace mtcnn
} // namespace tinyeye

#endif
