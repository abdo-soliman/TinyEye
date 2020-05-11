#include "mtcnn.h"
#include <caffe/layers/memory_data_layer.hpp>

namespace mtcnn
{
caffe::Datum MTCNN::mat_to_datum(const Mat &cv_mat)
{
    caffe::Datum datum;
    if (cv_mat.empty())
        return datum;

    int channels = cv_mat.channels();

    datum.set_channels(cv_mat.channels());
    datum.set_height(cv_mat.rows);
    datum.set_width(cv_mat.cols);
    datum.set_label(0);
    datum.clear_data();
    datum.clear_float_data();
    datum.set_encoded(false);

    int datum_height = datum.height();
    int datum_width = datum.width();
    if (channels == 3)
    {
        for (int c = 0; c < channels; c++)
        {
            for (int h = 0; h < datum_height; ++h)
            {
                for (int w = 0; w < datum_width; ++w)
                {
                    const float *ptr = cv_mat.ptr<float>(h);
                    datum.add_float_data(ptr[w * channels + c]);
                }
            }
        }
    }

    return datum;
}

vector<Bbox> MTCNN::stage2(Mat img)
{
    std::vector<caffe::Datum> datum_vector;

    auto mem_data_layer = boost::static_pointer_cast<caffe::MemoryDataLayer<float>>(r_net->layers()[0]);
    int input_width = mem_data_layer->width();
    int input_height = mem_data_layer->height();
    int pad_top, pad_left, pad_right, pad_bottom;
    Mat crop_img;

    // load crop_img data to datum
    for (size_t i = 0; i < regrec_1_result.size(); i++)
    {
        pad_top = std::abs(padding_1_result[i].x1 - regrec_1_result[i].x1);
        pad_left = std::abs(padding_1_result[i].y1 - regrec_1_result[i].y1);
        pad_right = std::abs(padding_1_result[i].y2 - regrec_1_result[i].y2);
        pad_bottom = std::abs(padding_1_result[i].x2 - regrec_1_result[i].x2);

        crop_img = img(cv::Range(padding_1_result[i].y1 - 1, padding_1_result[i].y2),
                       cv::Range(padding_1_result[i].x1 - 1, padding_1_result[i].x2));
        cv::copyMakeBorder(crop_img, crop_img, pad_left, pad_right,
                           pad_top, pad_bottom, cv::BORDER_CONSTANT, cv::Scalar(0));

        cv::resize(crop_img, crop_img, cv::Size(input_width, input_height), 0, 0, cv::INTER_AREA);
        crop_img = (crop_img - 127.5) * 0.0078125;

        datum_vector.push_back(mat_to_datum(crop_img));
        crop_img.release();
    }

    /* extract the features and store */
    mem_data_layer->set_batch_size(regrec_1_result.size());
    mem_data_layer->AddDatumVector(datum_vector);
    /* fire the network */
    float no_use_loss = 0.0f;
    r_net->Forward(&no_use_loss);

    const boost::shared_ptr<caffe::Blob<float>> reg = r_net->blob_by_name("conv5-2");
    const boost::shared_ptr<caffe::Blob<float>> confidence = r_net->blob_by_name("prob1");

    const float *confidence_data = confidence->cpu_data();
    const float *reg_data = reg->cpu_data();

    vector<Bbox> result, nms_union_result, nms_min_result;
    size_t index = 0;
    int size = static_cast<int>(regrec_1_result.size());
    for (int i = 0; i < size; i++)
    {
        if (*(confidence_data + i * 2 + 1) > steps_threshold[1])
        {
            Bbox box;
            box.x1 = regrec_1_result[index].x1;
            box.y1 = regrec_1_result[index].y1;
            box.x2 = regrec_1_result[index].x2;
            box.y2 = regrec_1_result[index].y2;

            box.width = box.x2 - box.x1 + 1;
            box.height = box.y2 - box.y1 + 1;

            box.score = *(confidence_data + i * 2 + 1);
            box.reg = cv::Vec4f(reg_data[4 * i + 0], reg_data[4 * i + 1],
                                          reg_data[4 * i + 2], reg_data[4 * i + 3]);

            result.push_back(box);
        }
        ++index;
    }

    if (result.size() > 0)
    {
        non_maximum_suppression(result, 0.7, 'u', nms_union_result);
        box_regress(nms_union_result, 2);
        non_maximum_suppression(nms_union_result, 0.7, 'm', nms_min_result);
    }

    return nms_min_result;
}
} // namespace mtcnn
