#include "mtcnn.h"

namespace tinyeye
{
namespace mtcnn
{
Mat MTCNN::scale_image(Mat image, double scale)
{
	int width_scaled = static_cast<int>(std::ceil(image.cols * scale));
	int height_scaled = static_cast<int>(std::ceil(image.rows * scale));

	Mat scaled;
	cv::resize(image, scaled, cv::Size(width_scaled, height_scaled), 0, 0, cv::INTER_AREA);
	scaled.convertTo(scaled, CV_32FC3, 0.0078125, -127.5 * 0.0078125);

	return scaled;
}

MTCNN::MTCNN(const std::string &models_dir, float thresholds[2], uint32_t min_fs, double sf)
{
#ifdef CPU_ONLY
	caffe::Caffe::set_mode(caffe::Caffe::CPU);
#else
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif

	// load the networks
	p_net.reset(new caffe::Net<float>((models_dir + "/pnet.prototxt"), caffe::TEST));
	p_net->CopyTrainedLayersFrom(models_dir + "/pnet.caffemodel");

	CHECK_EQ(p_net->num_inputs(), 1) << "Network should have exactly one input.";
	CHECK_EQ(p_net->num_outputs(), 2) << "Network should have exactly two output, one"
										 " is bbox and another is confidence.";

	r_net.reset(new caffe::Net<float>((models_dir + "/rnet.prototxt"), caffe::TEST));
	r_net->CopyTrainedLayersFrom(models_dir + "/rnet.caffemodel");

	caffe::Blob<float> *input_layer;
	input_layer = p_net->input_blobs()[0];
	num_channels = input_layer->channels();
	CHECK(num_channels == 3 || num_channels == 1) << "Input layer should have 1 or 3 channels.";

	if (thresholds == NULL)
	{
		steps_threshold[0] = 0.6;
		steps_threshold[1] = 0.7;
	}

	min_face_size = min_fs;
	scale_factor = sf;
}

void MTCNN::compute_scale_pyramid(double m, int min_layer, vector<double> &scales)
{
    int factor_count = 0;
    while (min_layer >= 12)
    {
        scales.push_back(m * std::pow(scale_factor, factor_count++));
        min_layer *= scale_factor;
    }
}

vector<Bbox> MTCNN::detect(Mat img)
{
	img.convertTo(img, CV_32FC3);
	cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
	img = img.t();

	int width = img.rows;
	int height = img.cols;

	double m = 12.0f / min_face_size;
	int min_layer = (height < width) ? height * m : width * m;
	vector<double> scales;
	compute_scale_pyramid(m, min_layer, scales);

	// stage 1 running P Net
	stage1(img, scales);
	// stage 2 running R Net
	return stage2(img);
}

std::list<cv::Rect2d> MTCNN::detect_rects(Mat img, float threshold)
{
	vector<Bbox> boxes = detect(img.clone());

	// createing vector of faces from boxes
	std::list<cv::Rect2d> results;
	int x1, y1, width, height;
	for (const auto &box : boxes)
	{
		if (box.score > threshold)
		{
			x1 = std::max(0, box.x1);
			y1 = std::max(0, box.y1);
			width = std::min(img.cols - y1, box.y2 - box.y1 + 1);
			height = std::min(img.rows - x1, box.x2 - box.x1 + 1);
			results.push_back(cv::Rect2d(y1, x1, width, height));
		}
	}

	return results;
}

vector<Mat> MTCNN::detect_faces(Mat img, float threshold)
{
	vector<Bbox> boxes = detect(img.clone());

	// createing vector of faces from boxes
	vector<Mat> results;
	int x1, y1, width, height;
	for (const auto &box : boxes)
	{
		if (box.score > threshold)
		{
			x1 = std::max(0, box.x1);
			y1 = std::max(0, box.y1);
			width = std::min(img.cols - y1, box.y2 - box.y1 + 1);
			height = std::min(img.rows - x1, box.x2 - box.x1 + 1);
			results.push_back(Mat(img, cv::Rect(y1, x1, width, height)));
		}
	}

	return results;
}

void MTCNN::detect_draw(Mat &img)
{
	vector<Bbox> boxes = detect(img.clone());
	int x1, y1, width, height;
	for (const auto box : boxes)
	{
		x1 = std::max(0, box.x1);
		y1 = std::max(0, box.y1);
		width = std::min(img.cols - y1, box.y2 - box.y1 + 1);
		height = std::min(img.rows - x1, box.x2 - box.x1 + 1);
		cv::rectangle(img, cv::Rect(y1, x1, width, height), cv::Scalar(255, 0, 0), 2);
	}
}
} // namespace mtcnn
} // tinyeye
