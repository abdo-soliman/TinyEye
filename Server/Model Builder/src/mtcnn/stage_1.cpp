#include "mtcnn.h"

namespace tinyeye
{
namespace mtcnn
{
void MTCNN::wrap_input_layer(vector<cv::Mat> *input_channels, caffe::Blob<float> *input_layer,
                             const int height, const int width)
{
    float *input_data = input_layer->mutable_cpu_data();
    for (int i = 0; i < input_layer->channels(); ++i)
    {
        Mat channel(height, width, CV_32FC1, input_data);
        input_channels->push_back(channel);
        input_data += width * height;
    }
}

void MTCNN::generate_bbox(caffe::Blob<float> *confidence, caffe::Blob<float> *reg,
                          double scale, float threshold, int image_width, int image_height, vector<Bbox> &boxes)
{
    int stride = 2;
    int cell_size = 12;

    int curr_feature_map_w = std::ceil((image_width - cell_size) * 1.0 / stride) + 1;
    int curr_feature_map_h = std::ceil((image_height - cell_size) * 1.0 / stride) + 1;
    int reg_offset = curr_feature_map_w * curr_feature_map_h;

    int count = confidence->count() / 2;
    const float *confidence_data = confidence->cpu_data();
    confidence_data += count;
    const float *reg_data = reg->cpu_data();

    int x, y;
    for (int i = 0; i < count; i++)
    {
        if (*(confidence_data + i) >= threshold)
        {
            y = i / curr_feature_map_w;
            x = i - curr_feature_map_w * y;

            Bbox box;
            box.x1 = static_cast<int>((x * stride + 1) / scale);
            box.x2 = static_cast<int>((x * stride + cell_size) / scale);
            box.y1 = static_cast<int>((y * stride + 1) / scale);
            box.y2 = static_cast<int>((y * stride + cell_size) / scale);

            box.width = box.x2 - box.x1 + 1;
            box.height = box.y2 - box.y1 + 1;

            box.score = *(confidence_data + i);
            box.reg = cv::Vec4f(reg_data[i + 0 * reg_offset], reg_data[i + 1 * reg_offset],
                                         reg_data[i + 2 * reg_offset], reg_data[i + 3 * reg_offset]);

            boxes.push_back(box);
        }
    }
}

void MTCNN::stage1(Mat img, const vector<double> &scales)
{
    Mat scaled_img;
    vector<Mat> input_channels;
    vector<Bbox> boxes, result, final_result;
    caffe::Blob<float> *input_layer = p_net->input_blobs()[0];
    for (const auto &scale : scales)
    {
        scaled_img = scale_image(img, scale);
        input_layer->Reshape(1, 3, scaled_img.rows, scaled_img.cols);
        p_net->Reshape();

        wrap_input_layer(&input_channels, p_net->input_blobs()[0], scaled_img.rows, scaled_img.cols);
        cv::split(scaled_img, input_channels);

        // check data transform right
        CHECK(reinterpret_cast<float *>(input_channels.at(0).data) == p_net->input_blobs()[0]->cpu_data())
            << "Input channels are not wrapping the input layer of the network.";
        p_net->Forward();

        caffe::Blob<float> *reg = p_net->output_blobs()[0];
        caffe::Blob<float> *confidence = p_net->output_blobs()[1];

        generate_bbox(confidence, reg, scale, steps_threshold[0], scaled_img.cols, scaled_img.rows, boxes);
        non_maximum_suppression(boxes, 0.5, 'u', result);

        input_channels.clear();
        boxes.clear();
    }

    if (result.size() > 0)
    {
        non_maximum_suppression(result, 0.7, 'u', final_result);
        box_regress(final_result, 1);
        box_to_square(final_result);
        padding(regrec_1_result, img.rows, img.cols);
    }
}
} // namespace mtcnn
} // namepace tinyeye
